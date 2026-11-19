
// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <net/if.h>
#include <net/if_utun.h>
#include <net/route.h>
#include <netinet/in.h>
#include <poke/make_error.hpp>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/socket.h>
#include <sys/sys_domain.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "../../interface.hpp"
#include "../../log_level.hpp"
#include "../action.hpp"
#include "../monitor.hpp"
#include "../scoped_file_descriptor.hpp"
#include "../to_json_property.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_macos
{

class interface
{
public:
    interface() : m_monitor("macos_tunnel_interface", {}, action::poke_registry)
    {
    }

    // create the utun device
    void create(const tunnel::interface::config& config, std::error_code& error)
    {
        assert(!m_interface_fd && "Cannot create an already created device.");

        if (config.interface_type != tunnel::interface::type::tun)
        {
            do_log(log_level::error, action::open,
                   poke::log::str{"error", "Only TUN is supported on MacOS"});
            error = std::make_error_code(std::errc::not_supported);
            return;
        }

        scoped_file_descriptor control_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (!control_fd)
        {
            error = last_errno();
            do_log(
                log_level::error, action::open,
                poke::make_error(error, poke::log::str{"step", "control_fd"}));
            return;
        }
        scoped_file_descriptor route_fd = socket(AF_ROUTE, SOCK_RAW, AF_INET);
        if (!route_fd)
        {
            error = last_errno();
            do_log(log_level::error, action::open,
                   poke::make_error(error, poke::log::str{"step", "route_fd"}));
            return;
        }

        struct ctl_info ctlInfo;
        strlcpy(ctlInfo.ctl_name, UTUN_CONTROL_NAME, sizeof(ctlInfo.ctl_name));

        scoped_file_descriptor interface_fd =
            socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
        if (!interface_fd)
        {
            error = last_errno();
            do_log(log_level::error, action::open,
                   poke::make_error(error,
                                    poke::log::str{"step", "interface_fd"}));
            return;
        }

        // Get the control ID
        struct sockaddr_ctl sc;
        memset(&sc, 0, sizeof(sc));
        if (ioctl(interface_fd.native_handle(), CTLIOCGINFO, &ctlInfo) == -1)
        {
            error = last_errno();
            do_log(log_level::error, action::open,
                   poke::make_error(error,
                                    poke::log::str{"step", "get_control_id"}));
            return;
        }

        // Let the kernel pick the first free unit (sc_unit = 0). Probing for
        // a free unit ourselves is racy, and the low units are typically
        // already taken by system services and VPNs.
        sc.sc_id = ctlInfo.ctl_id;
        sc.sc_len = sizeof(sc);
        sc.sc_family = AF_SYSTEM;
        sc.ss_sysaddr = AF_SYS_CONTROL;
        sc.sc_unit = 0;

        if (connect(interface_fd.native_handle(), (struct sockaddr*)&sc,
                    sizeof(sc)) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::open,
                   poke::make_error(error, poke::log::str{"step", "connect"}));
            return;
        }

        // Set non-blocking mode
        if (fcntl(interface_fd.native_handle(), F_SETFL, O_NONBLOCK) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::open,
                   poke::make_error(
                       error, poke::log::str{"step", "set_non_blocking"}));
            return;
        }

        // Ask the kernel which name the interface got (utun<N>)
        char name[IFNAMSIZ];
        memset(name, 0, sizeof(name));
        socklen_t name_size = sizeof(name);
        if (getsockopt(interface_fd.native_handle(), SYSPROTO_CONTROL,
                       UTUN_OPT_IFNAME, name, &name_size) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::open,
                   poke::make_error(
                       error, poke::log::str{"step", "get_interface_name"}));
            return;
        }
        m_name = name;

        // Set the interface file descriptors
        m_interface_fd = std::move(interface_fd);
        m_control_fd = std::move(control_fd);
        m_route_fd = std::move(route_fd);
    }

    auto make_ifreq() const -> struct ifreq
    {
        assert(!m_name.empty() && "Device name is empty.");
        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, m_name.c_str(), IFNAMSIZ);
        return ifr;
    }

    // Check if the device is open
    bool is_open() const
    {
        return static_cast<bool>(m_interface_fd);
    }

    // Get the file descriptor
    int native_handle() const
    {
        return m_interface_fd.native_handle();
    }

    // Set IP address and netmask
    void set_ipv4(const std::string& ipAddress, std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd && "Control socket is not open.");
        assert(!error);

        auto ifr = make_ifreq();

        // Set IP address
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        const int result =
            inet_pton(AF_INET, ipAddress.c_str(), &addr->sin_addr);
        if (result <= 0)
        {
            error = inet_pton_error(result);
            do_log(log_level::error, action::set_ipv4, poke::make_error(error));
            return;
        }

        if (ioctl(m_control_fd.native_handle(), SIOCSIFADDR, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::set_ipv4, poke::make_error(error));
            return;
        }
    }

    void set_ipv4_netmask(const std::string& mask, std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd && "Control socket is not open.");
        assert(!error);

        auto ifr = make_ifreq();
        // Set IP address
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        const int result = inet_pton(AF_INET, mask.c_str(), &addr->sin_addr);
        if (result <= 0)
        {
            error = inet_pton_error(result);
            do_log(log_level::error, action::set_ipv4_netmask,
                   poke::make_error(error));
            return;
        }

        ifr.ifr_addr.sa_family = AF_INET;
        if (ioctl(m_control_fd.native_handle(), SIOCSIFNETMASK, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::set_ipv4_netmask,
                   poke::make_error(error));
            return;
        }
        m_netmask = mask;
    }
    auto ipv4(std::error_code& error) const -> std::string
    {
        assert(!error);

        struct ifreq ifr = make_ifreq();
        if (ioctl(m_control_fd.native_handle(), SIOCGIFADDR, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::ipv4, poke::make_error(error));
            return "";
        }

        struct sockaddr_in* addr_in = (struct sockaddr_in*)&ifr.ifr_addr;
        do_log(log_level::debug, action::ipv4,
               poke::log::str{"ip", ::inet_ntoa(addr_in->sin_addr)});

        return ::inet_ntoa(addr_in->sin_addr);
    }

    auto ipv4_netmask(std::error_code& error) const -> std::string
    {
        assert(!error);

        struct ifreq ifr = make_ifreq();
        if (ioctl(m_control_fd.native_handle(), SIOCGIFNETMASK, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::ipv4_netmask,
                   poke::make_error(error));
            return "";
        }

        struct sockaddr_in* addr_in = (struct sockaddr_in*)&ifr.ifr_addr;
        do_log(log_level::debug, action::ipv4_netmask,
               poke::log::str{"netmask", ::inet_ntoa(addr_in->sin_addr)});

        return ::inet_ntoa(addr_in->sin_addr);
    }

    // Set MTU
    void set_mtu(uint32_t mtu, std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd && "Control socket is not open.");
        auto ifr = make_ifreq();

        // Set MTU
        ifr.ifr_mtu = mtu;
        if (ioctl(m_control_fd.native_handle(), SIOCSIFMTU, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::set_mtu, poke::make_error(error));
            return;
        }
        m_mtu = mtu;
    }

    // Bring up the interface
    void up(std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd && "Control socket is not open.");

        auto ifr = make_ifreq();
        // Get current flags
        if (ioctl(m_control_fd.native_handle(), SIOCGIFFLAGS, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::interface_up,
                   poke::make_error(error));
            return;
        }

        // Set the interface up
        ifr.ifr_flags |= IFF_UP;
        if (ioctl(m_control_fd.native_handle(), SIOCSIFFLAGS, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::interface_up,
                   poke::make_error(error));
            return;
        }
    }
    auto is_up(std::error_code& error) const -> bool
    {
        assert(!error);

        struct ifreq ifr = make_ifreq();
        if (ioctl(m_control_fd.native_handle(), SIOCGIFFLAGS, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::is_up, poke::make_error(error));
            return false;
        }

        bool is_if_up = (ifr.ifr_flags & IFF_UP) != 0;

        do_log(log_level::debug, action::is_up,
               log::boolean{"is_up", is_if_up});

        return is_if_up;
    }

    // Bring down the interface
    void down(std::error_code& error) const
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd && "Control socket is not open.");

        auto ifr = make_ifreq();
        // Get current flags
        if (ioctl(m_control_fd.native_handle(), SIOCGIFFLAGS, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::interface_down,
                   poke::make_error(error));
            return;
        }

        // Set the interface down
        ifr.ifr_flags &= ~IFF_UP;
        if (ioctl(m_control_fd.native_handle(), SIOCSIFFLAGS, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::interface_down,
                   poke::make_error(error));
            return;
        }
    }
    auto is_down(std::error_code& error) const -> bool
    {
        assert(!error);
        do_log(log_level::debug, action::is_down);
        return !is_up(error);
    }

    // Set the default route
    void enable_default_route(std::error_code& error)
    {
        assert(!error);
        assert(m_route_fd && "Route socket is not open.");
        assert(!m_name.empty() && "Interface name is empty.");

        // The interface address is used as the gateway
        const std::string gateway = ipv4(error);
        if (error)
        {
            return;
        }

        struct
        {
            struct rt_msghdr hdr;
            struct sockaddr_in dst;
            struct sockaddr_in gw;
            struct sockaddr_in netmask;
        } rtmsg;

        memset(&rtmsg, 0, sizeof(rtmsg));

        rtmsg.hdr.rtm_msglen = sizeof(rtmsg);
        rtmsg.hdr.rtm_version = RTM_VERSION;
        rtmsg.hdr.rtm_type = RTM_ADD;
        rtmsg.hdr.rtm_flags = RTF_UP | RTF_GATEWAY | RTF_STATIC | RTF_IFSCOPE;
        rtmsg.hdr.rtm_addrs = RTA_DST | RTA_GATEWAY | RTA_NETMASK;
        rtmsg.hdr.rtm_seq = 1;
        rtmsg.hdr.rtm_pid = getpid();

        // Destination is 0.0.0.0 (default route)
        rtmsg.dst.sin_len = sizeof(struct sockaddr_in);
        rtmsg.dst.sin_family = AF_INET;
        rtmsg.dst.sin_addr.s_addr = 0;

        // Gateway
        rtmsg.gw.sin_len = sizeof(struct sockaddr_in);
        rtmsg.gw.sin_family = AF_INET;
        const int result =
            inet_pton(AF_INET, gateway.c_str(), &rtmsg.gw.sin_addr);
        if (result <= 0)
        {
            error = inet_pton_error(result);
            do_log(log_level::error, action::enable_default_route,
                   poke::make_error(error));
            return;
        }

        // Netmask is 0.0.0.0
        rtmsg.netmask.sin_len = sizeof(struct sockaddr_in);
        rtmsg.netmask.sin_family = AF_INET;
        rtmsg.netmask.sin_addr.s_addr = htonl(INADDR_ANY);

        // set the interface to be the default route

        rtmsg.hdr.rtm_index = if_nametoindex(m_name.c_str());

        // Send the message to the routing socket
        if (write(m_route_fd.native_handle(), &rtmsg, sizeof(rtmsg)) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::enable_default_route,
                   poke::make_error(error));
            return;
        }
        m_is_default_route = true;
    }

    // remove the default route
    void disable_default_route(std::error_code& error)
    {
        assert(!error);
        assert(m_route_fd && "Route socket is not open.");
        assert(!m_name.empty() && "Interface name is empty.");

        // The interface address is used as the gateway
        const std::string gateway = ipv4(error);
        if (error)
        {
            return;
        }

        struct
        {
            struct rt_msghdr hdr;
            struct sockaddr_in dst;
            struct sockaddr_in gw;
            struct sockaddr_in netmask;
        } rtmsg;

        memset(&rtmsg, 0, sizeof(rtmsg));

        rtmsg.hdr.rtm_msglen = sizeof(rtmsg);
        rtmsg.hdr.rtm_version = RTM_VERSION;
        rtmsg.hdr.rtm_type = RTM_DELETE;
        rtmsg.hdr.rtm_flags =
            RTF_GATEWAY | RTF_STATIC | RTF_IFSCOPE | RTF_CONDEMNED;
        rtmsg.hdr.rtm_addrs = RTA_DST | RTA_GATEWAY | RTA_NETMASK;
        rtmsg.hdr.rtm_seq = 1;
        rtmsg.hdr.rtm_pid = getpid();

        // Destination is 0.0.0.0 (default route)
        rtmsg.dst.sin_len = sizeof(struct sockaddr_in);
        rtmsg.dst.sin_family = AF_INET;
        rtmsg.dst.sin_addr.s_addr = 0;

        // Gateway
        rtmsg.gw.sin_len = sizeof(struct sockaddr_in);
        rtmsg.gw.sin_family = AF_INET;
        rtmsg.gw.sin_addr.s_addr = htonl(INADDR_ANY);
        const int result =
            inet_pton(AF_INET, gateway.c_str(), &rtmsg.gw.sin_addr);
        if (result <= 0)
        {
            error = inet_pton_error(result);
            do_log(log_level::error, action::disable_default_route,
                   poke::make_error(error));
            return;
        }

        // Netmask is 0.0.0.0
        rtmsg.netmask.sin_len = sizeof(struct sockaddr_in);
        rtmsg.netmask.sin_family = AF_INET;
        rtmsg.netmask.sin_addr.s_addr = htonl(INADDR_ANY);

        // set the interface to be the default route

        rtmsg.hdr.rtm_index = if_nametoindex(m_name.c_str());

        // Send the message to the routing socket
        if (write(m_route_fd.native_handle(), &rtmsg, sizeof(rtmsg)) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::disable_default_route,
                   poke::make_error(error));
            return;
        }
        m_is_default_route = false;
    }

    std::string interface_name(std::error_code& error) const
    {
        assert(!m_name.empty() && "Interface name is empty.");
        return m_name;
    }

    auto mtu(std::error_code& error) const -> uint32_t
    {
        assert(!error);
        auto ifr = make_ifreq();
        if (ioctl(m_control_fd.native_handle(), SIOCGIFMTU, &ifr) < 0)
        {
            error = last_errno();
            do_log(log_level::error, action::interface_mtu,
                   poke::make_error(error));
            return 0;
        }
        do_log(log_level::debug, action::interface_mtu,
               poke::log::integer{"mtu", ifr.ifr_mtu});

        return ifr.ifr_mtu;
    }

    bool is_default_route(std::error_code& error) const
    {
        return m_is_default_route;
    }

    // Unsupported methods for MacOS
    void rename(const std::string&, std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_owner(const std::string&, std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string&, std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_offload(const tunnel::interface::offload&,
                     std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_non_persistent(std::error_code& error) const
    {
        assert(!error);
        do_log(log_level::error, action::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    auto monitor() -> tunnel::monitor&
    {
        return m_monitor;
    }

    auto monitor() const -> const tunnel::monitor&
    {
        return m_monitor;
    }

protected:
    /// @return The current errno as an error code. Call this before anything
    ///         else that could overwrite errno, e.g. logging.
    static auto last_errno() -> std::error_code
    {
        return std::error_code(errno, std::generic_category());
    }

    /// @return The error for a failed inet_pton call. A return value of 0
    ///         means the string was not a valid address, in which case errno
    ///         is not set.
    static auto inet_pton_error(int result) -> std::error_code
    {
        assert(result <= 0);
        return result == 0 ? std::make_error_code(std::errc::invalid_argument)
                           : last_errno();
    }

    template <class Kind, class... Args>
    void do_log(log_level level, const Kind& kind, Args&&... args) const
    {
        m_monitor.m_monitor.log(static_cast<poke::log_level>(level), kind,
                                std::forward<const Args>(args)...);
    }

private:
    tunnel::detail::monitor m_monitor;
    scoped_file_descriptor m_interface_fd;
    scoped_file_descriptor m_control_fd;
    scoped_file_descriptor m_route_fd;
    std::string m_name;
    std::string m_ipAddress;
    std::string m_netmask;
    uint32_t m_mtu = -1;
    bool m_is_default_route = false;
};

}
}
}
