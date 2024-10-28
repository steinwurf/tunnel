
// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <net/if.h>
#include <net/if_utun.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sys_domain.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h> // strlcpy

#include <net/if_utun.h>      // UTUN_CONTROL_NAME
#include <sys/ioctl.h>        // ioctl
#include <sys/kern_control.h> // struct socketaddr_ctl

#include "../../interface_config.hpp"
#include "../log_kind.hpp"
#include "../monitor.hpp"
#include "../to_json_property.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_macos
{

template <class Super>
class layer_interface : public Super
{
public:
    ~layer_interface()
    {
        cleanup();
    }

    // create the utun device
    void create(const config& config, std::error_code& error)
    {
        assert(m_interface_fd == -1 &&
               "Cannot create an already created device.");

        m_control_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_control_fd < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            cleanup();
            return;
        }
        m_route_fd = socket(AF_ROUTE, SOCK_RAW, AF_INET);
        if (m_route_fd < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            cleanup();
            return;
        }

        struct ctl_info ctlInfo;
        strlcpy(ctlInfo.ctl_name, UTUN_CONTROL_NAME, sizeof(ctlInfo.ctl_name));

        m_interface_fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
        if (m_interface_fd < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            cleanup();
            return;
        }

        struct sockaddr_ctl sc;
        memset(&sc, 0, sizeof(sc));
        if (ioctl(m_interface_fd, CTLIOCGINFO, &ctlInfo) == -1)
        {
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            cleanup();
            return;
        }

        // Find a free unit number
        auto find_free_unit = [&]() -> int
        {
            int sock = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
            if (sock < 0)
            {
                Super::do_log(poke::log_level::error, log_kind::open,
                              poke::log::str{"error", strerror(errno)});
                error = std::make_error_code(std::errc::io_error);
                return -1;
            }

            for (int i = 7; i < 256; ++i)
            {
                struct sockaddr_ctl sc;
                memset(&sc, 0, sizeof(sc));
                sc.sc_len = sizeof(sc);
                sc.sc_family = AF_SYSTEM;
                sc.ss_sysaddr = AF_SYS_CONTROL;
                sc.sc_id = -1;
                sc.sc_unit = i;

                if (connect(sock, (struct sockaddr*)&sc, sizeof(sc)) < 0)
                {
                    close(sock);
                    return i;
                }
                close(sock);
            }
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", "No free unit found"});
            error = std::make_error_code(std::errc::io_error);
            return -1;
        };

        // Scan for a free unit number
        m_unit = find_free_unit();
        if (m_unit == -1)
        {
            cleanup();
            return;
        }

        sc.sc_id = ctlInfo.ctl_id;
        sc.sc_len = sizeof(sc);
        sc.sc_family = AF_SYSTEM;
        sc.ss_sysaddr = AF_SYS_CONTROL;
        sc.sc_unit = m_unit;

        if (connect(m_interface_fd, (struct sockaddr*)&sc, sizeof(sc)) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            cleanup();
            return;
        }

        // Set non-blocking mode
        if (fcntl(m_interface_fd, F_SETFL, O_NONBLOCK) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::open,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            cleanup();
            return;
        }

        // Construct the interface name
        m_name = "utun" + std::to_string(m_unit - 1);
    }

    // Close the utun device
    void cleanup()
    {
        if (m_control_fd != -1)
        {
            ::close(m_control_fd);
        }
        if (m_interface_fd != -1)
        {
            ::close(m_interface_fd);
            m_interface_fd = -1;
        }
        if (m_route_fd != -1)
        {
            ::close(m_route_fd);
            m_route_fd = -1;
        }
        m_unit = -1;
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
    bool
    is_open() const
    {
        return m_interface_fd != -1;
    }

    // Get the file descriptor
    int native_handle() const
    {
        return m_interface_fd;
    }

    // Set IP address and netmask
    void set_ipv4(const std::string& ipAddress, std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd != -1 && "Control socket is not open.");
        assert(!error);

        auto ifr = make_ifreq();

        // Set IP address
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        if (inet_pton(AF_INET, ipAddress.c_str(), &addr->sin_addr) <= 0)
        {
            Super::do_log(poke::log_level::error, log_kind::set_ipv4,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        if (ioctl(m_control_fd, SIOCSIFADDR, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::set_ipv4,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
    }

    void set_ipv4_netmask(const std::string& mask, std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd != -1 && "Control socket is not open.");
        assert(!error);

        auto ifr = make_ifreq();
        // Set IP address
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        if (inet_pton(AF_INET, mask.c_str(), &addr->sin_addr) <= 0)
        {
            Super::do_log(poke::log_level::error, log_kind::set_ipv4_netmask,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        ifr.ifr_addr.sa_family = AF_INET;
        if (ioctl(m_control_fd, SIOCSIFNETMASK, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::set_ipv4_netmask,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
        m_netmask = mask;
    }
    auto ipv4(std::error_code& error) const -> std::string
    {
        assert(!error);

        struct ifreq ifr = make_ifreq();
        ioctl(m_control_fd, SIOCGIFADDR, &ifr, error);
        if (error)
        {
            return "";
        }

        struct sockaddr_in* addr_in = (struct sockaddr_in*)&ifr.ifr_addr;
        Super::do_log(poke::log_level::debug, log_kind::interface_ipv4,
                      poke::log::str{"ip", ::inet_ntoa(addr_in->sin_addr)});

        return ::inet_ntoa(addr_in->sin_addr);
    }

    auto ipv4_netmask(std::error_code& error) const -> std::string
    {
        assert(!error);

        struct ifreq ifr = make_ifreq();
        ioctl(m_control_fd, SIOCGIFNETMASK, &ifr, error);
        if (error)
        {
            return "";
        }

        struct sockaddr_in* addr_in = (struct sockaddr_in*)&ifr.ifr_addr;
        Super::do_log(
            poke::log_level::debug, log_kind::interface_ipv4_netmask,
            poke::log::str{"netmask", ::inet_ntoa(addr_in->sin_addr)});

        return ::inet_ntoa(addr_in->sin_addr);
    }

    // Set MTU
    void set_mtu(uint32_t mtu, std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd != -1 && "Control socket is not open.");
        auto ifr = make_ifreq();

        // Set MTU
        ifr.ifr_mtu = mtu;
        if (ioctl(m_control_fd, SIOCSIFMTU, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::set_mtu,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
        m_mtu = mtu;
    }

    // Bring up the interface
    void up(std::error_code& error)
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd != -1 && "Control socket is not open.");

        auto ifr = make_ifreq();
        // Get current flags
        if (ioctl(m_control_fd, SIOCGIFFLAGS, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::interface_up,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        // Set the interface up
        ifr.ifr_flags |= IFF_UP;
        if (ioctl(m_control_fd, SIOCSIFFLAGS, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::interface_up,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
    }
    auto is_up(std::error_code& error) const -> bool
    {
        assert(!error);

        struct ifreq ifr = make_ifreq();
        ioctl(m_control_fd, SIOCGIFFLAGS, &ifr, error);

        if (error)
        {
            return false;
        }

        bool is_if_up = (ifr.ifr_flags & IFF_UP) != 0;

        Super::do_log(poke::log_level::debug, log_kind::is_up,
                      log::boolean{"is_up", is_if_up});

        return is_if_up;
    }

    // Bring down the interface
    void down(std::error_code& error) const
    {
        assert(is_open() && "Device is not open.");
        assert(m_control_fd != -1 && "Control socket is not open.");

        auto ifr = make_ifreq();
        // Get current flags
        if (ioctl(m_control_fd, SIOCGIFFLAGS, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::interface_down,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        // Set the interface down
        ifr.ifr_flags &= ~IFF_UP;
        if (ioctl(m_control_fd, SIOCSIFFLAGS, &ifr) < 0)
        {
            Super::do_log(poke::log_level::error, log_kind::interface_down,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
    }
    auto is_down(std::error_code& error) const -> bool
    {
        assert(!error);
        Super::do_log(poke::log_level::debug, log_kind::is_down);
        return !is_up(error);
    }

    // Set the default route
    void enable_default_route(std::error_code& error)
    {
        assert(!error);
        assert(m_route_fd != -1 && "Route socket is not open.");
        assert(!ipv4(error).empty() && "IP address is empty.");
        assert(!m_name.empty() && "Interface name is empty.");
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
        if (inet_pton(AF_INET, ipv4(error).c_str(), &rtmsg.gw.sin_addr) <= 0)
        {
            Super::do_log(poke::log_level::error,
                          log_kind::enable_default_route,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        // Netmask is 0.0.0.0
        rtmsg.netmask.sin_len = sizeof(struct sockaddr_in);
        rtmsg.netmask.sin_family = AF_INET;
        rtmsg.netmask.sin_addr.s_addr = htonl(INADDR_ANY);

        // set the interface to be the default route

        rtmsg.hdr.rtm_index = if_nametoindex(m_name.c_str());

        // Send the message to the routing socket
        if (write(m_route_fd, &rtmsg, sizeof(rtmsg)) < 0)
        {
            Super::do_log(poke::log_level::error,
                          log_kind::enable_default_route,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
        m_is_default_route = true;
    }

    // remove the default route
    void disable_default_route(std::error_code& error)
    {
        assert(!error);
        assert(m_route_fd != -1 && "Route socket is not open.");
        assert(!ipv4(error).empty() && "IP address is empty.");
        assert(!m_name.empty() && "Interface name is empty.");
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
        if (inet_pton(AF_INET, ipv4(error).c_str(), &rtmsg.gw.sin_addr) <= 0)
        {
            Super::do_log(poke::log_level::error,
                          log_kind::enable_default_route,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        // Netmask is 0.0.0.0
        rtmsg.netmask.sin_len = sizeof(struct sockaddr_in);
        rtmsg.netmask.sin_family = AF_INET;
        rtmsg.netmask.sin_addr.s_addr = htonl(INADDR_ANY);

        // set the interface to be the default route

        rtmsg.hdr.rtm_index = if_nametoindex(m_name.c_str());

        // Send the message to the routing socket
        if (write(m_route_fd, &rtmsg, sizeof(rtmsg)) < 0)
        {
            Super::do_log(poke::log_level::error,
                          log_kind::enable_default_route,
                          poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
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
        auto ifr = make_ifreq();
        ioctl(m_control_fd, SIOCGIFMTU, &ifr, error);
        if (error)
        {
            return 0;
        }
        Super::do_log(poke::log_level::debug, log_kind::interface_mtu,
                      poke::log::integer{"mtu", ifr.ifr_mtu});

        return ifr.ifr_mtu;
    }

    bool is_default_route(std::error_code& error) const
    {
        return m_is_default_route;
    }

    // Unsupported methods for MacOS
    void rename(const std::string& new_name, std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_owner(const std::string& owner, std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string& group, std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_non_persistent(std::error_code& error) const
    {
        Super::do_log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

private:
    int m_interface_fd = -1;
    int m_control_fd = -1;
    int m_route_fd = -1;
    int m_unit = -1; // unit number of the utun device
    std::string m_name;
    std::string m_ipAddress;
    std::string m_netmask;
    uint32_t m_mtu = -1;
    bool m_is_default_route = false;
};

}
}
}