// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <linux/if_ether.h>
#include <linux/route.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <system_error>

#include "../log.hpp"
#include "../log_kind.hpp"

#include "error.hpp"
#include "scoped_file_descriptor.hpp"

#include "../../log_level.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_linux
{
/// netdevice API:
/// http://man7.org/linux/man-pages/man7/netdevice.7.html
template <class Super>
class layer_netdevice : public Super
{
public:
    void create(const std::string& interface_name, std::error_code& error)
    {
        assert(!error);

        Super::create(interface_name, error);

        if (error)
        {
            return;
        }

        m_dev_fd = Super::socket(AF_INET, SOCK_STREAM, 0, error);
    }
    void create(const std::string& interface_name, std::error_code& error,
                bool vnet_hdr)
    {
        assert(!error);

        Super::create(interface_name, error, vnet_hdr);

        if (error)
        {
            return;
        }

        m_dev_fd = Super::socket(AF_INET, SOCK_STREAM, 0, error);
    }

    void rename(const std::string& interface_name, std::error_code& error) const
    {
        assert(!error);
        assert(interface_name.size() > 0);

        if (interface_name.size() > IFNAMSIZ - 1)
        {
            error = make_error_code(linux_error::interface_name_too_long);
            Super::do_log(log_level::error, log_kind::interface_renamed,
                          log::str{"interface_name", interface_name.c_str()},
                          log::str{"error", error.message().c_str()});
            return;
        }

        bool iface_up = is_up(error);

        if (error)
        {
            return;
        }

        if (iface_up)
        {
            down(error);

            if (error)
            {
                return;
            }
        }

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return;
        }

        interface_name.copy(ifr.ifr_newname, interface_name.size());

        Super::ioctl(m_dev_fd, SIOCSIFNAME, &ifr, error);

        if (error)
        {
            return;
        }

        Super::do_log(log_level::debug, log_kind::interface_renamed,
                      log::str{"interface_name", interface_name.c_str()});

        if (iface_up)
        {
            up(error);
        }
    }

    auto is_up(std::error_code& error) const -> bool
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return false;
        }

        Super::ioctl(m_dev_fd, SIOCGIFFLAGS, &ifr, error);

        if (error)
        {
            return false;
        }

        bool is_if_up = (ifr.ifr_flags & IFF_UP) != 0;

        Super::do_log(log_level::debug, log_kind::is_up,
                      log::boolean{"is_up", is_if_up});

        return is_if_up;
    }

    void up(std::error_code& error) const
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return;
        }

        Super::ioctl(m_dev_fd, SIOCGIFFLAGS, &ifr, error);

        if (error)
        {
            return;
        }

        ifr.ifr_flags |= IFF_UP;
        Super::ioctl(m_dev_fd, SIOCSIFFLAGS, &ifr, error);

        Super::do_log(log_level::debug, log_kind::interface_up);
    }

    auto is_down(std::error_code& error) const -> bool
    {
        assert(!error);
        Super::do_log(log_level::debug, log_kind::is_down);
        return !is_up(error);
    }

    void down(std::error_code& error) const
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return;
        }

        Super::ioctl(m_dev_fd, SIOCGIFFLAGS, &ifr, error);

        if (error)
        {
            return;
        }

        ifr.ifr_flags &= ~IFF_UP;
        Super::ioctl(m_dev_fd, SIOCSIFFLAGS, &ifr, error);

        Super::do_log(log_level::debug, log_kind::interface_down);
    }

    void set_mtu(uint32_t mtu, std::error_code& error) const
    {
        assert(!error);

        if (mtu < ETH_HLEN || mtu > 65535)
        {
            error = make_error_code(linux_error::mtu_too_large);
            Super::do_log(log_level::error, log_kind::set_mtu,
                          log::uinteger{"mtu", mtu},
                          log::str{"error", error.message().c_str()});
            return;
        }

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return;
        }

        ifr.ifr_mtu = mtu;

        Super::ioctl(m_dev_fd, SIOCSIFMTU, &ifr, error);

        Super::do_log(log_level::debug, log_kind::set_mtu,
                      log::integer{"mtu", mtu});
    }

    auto mtu(std::error_code& error) const -> uint32_t
    {
        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return 0;
        }

        Super::ioctl(m_dev_fd, SIOCGIFMTU, &ifr, error);

        Super::do_log(log_level::debug, log_kind::interface_mtu,
                      log::integer{"mtu", ifr.ifr_mtu});

        return ifr.ifr_mtu;
    }

    void enable_default_route(std::error_code& error) const
    {
        assert(!error);

        struct rtentry route
        {
        };

        std::string interface_name = Super::interface_name(error);

        if (error)
        {
            return;
        }

        char ifname[IFNAMSIZ] = {0};
        interface_name.copy(ifname, interface_name.size());

        route.rt_dev = ifname;
        route.rt_flags = RTF_UP;

        struct sockaddr_in* gateway = (struct sockaddr_in*)&route.rt_gateway;
        gateway->sin_family = AF_INET;
        gateway->sin_addr.s_addr = INADDR_ANY;

        struct sockaddr_in* dest = (struct sockaddr_in*)&route.rt_dst;
        dest->sin_family = AF_INET;
        dest->sin_addr.s_addr = INADDR_ANY;

        struct sockaddr_in* mask = (struct sockaddr_in*)&route.rt_genmask;
        mask->sin_family = AF_INET;
        mask->sin_addr.s_addr = INADDR_ANY;

        Super::ioctl(m_dev_fd, SIOCADDRT, &route, error);

        Super::do_log(log_level::debug, log_kind::enable_default_route);
    }

    void disable_default_route(std::error_code& error) const
    {
        assert(!error);

        struct rtentry route
        {
        };

        std::string interface_name = Super::interface_name(error);

        if (error)
        {
            return;
        }

        char ifname[IFNAMSIZ] = {0};
        interface_name.copy(ifname, interface_name.size());

        route.rt_dev = ifname;
        route.rt_flags = RTF_UP;

        struct sockaddr_in* gateway = (struct sockaddr_in*)&route.rt_gateway;
        gateway->sin_family = AF_INET;
        gateway->sin_addr.s_addr = INADDR_ANY;

        struct sockaddr_in* dest = (struct sockaddr_in*)&route.rt_dst;
        dest->sin_family = AF_INET;
        dest->sin_addr.s_addr = INADDR_ANY;

        struct sockaddr_in* mask = (struct sockaddr_in*)&route.rt_genmask;
        mask->sin_family = AF_INET;
        mask->sin_addr.s_addr = INADDR_ANY;

        Super::ioctl(m_dev_fd, SIOCDELRT, &route, error);

        Super::do_log(log_level::debug, log_kind::disable_default_route);
    }

    auto ipv4(std::error_code& error) const -> std::string
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return "";
        }

        Super::ioctl(m_dev_fd, SIOCGIFADDR, &ifr, error);

        if (error)
        {
            return "";
        }

        struct sockaddr_in* addr_in = (struct sockaddr_in*)&ifr.ifr_addr;

        Super::do_log(log_level::debug, log_kind::interface_ipv4,
                      log::str{"ip", ::inet_ntoa(addr_in->sin_addr)});

        return ::inet_ntoa(addr_in->sin_addr);
    }

    auto ipv4_netmask(std::error_code& error) const -> std::string
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return "";
        }

        Super::ioctl(m_dev_fd, SIOCGIFNETMASK, &ifr, error);

        if (error)
        {
            return "";
        }

        struct sockaddr_in* addr_in = (struct sockaddr_in*)&ifr.ifr_addr;

        Super::do_log(log_level::debug, log_kind::interface_ipv4_netmask,
                      log::str{"netmask", ::inet_ntoa(addr_in->sin_addr)});

        return ::inet_ntoa(addr_in->sin_addr);
    }

    void set_ipv4(const std::string& address, std::error_code& error) const
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return;
        }

        // Set the IP

        struct sockaddr addr = make_sockaddr(address, error);

        if (error)
        {
            return;
        }

        ifr.ifr_addr = addr;

        Super::ioctl(m_dev_fd, SIOCSIFADDR, &ifr, error);

        Super::do_log(log_level::debug, log_kind::set_ipv4,
                      log::str{"ip", address.c_str()});
    }

    void set_ipv4_netmask(const std::string& netmask,
                          std::error_code& error) const
    {
        assert(!error);

        struct ifreq ifr = make_ifreq(error);

        if (error)
        {
            return;
        }

        struct sockaddr mask = make_sockaddr(netmask, error);

        if (error)
        {
            return;
        }

        ifr.ifr_netmask = mask;

        Super::ioctl(m_dev_fd, SIOCSIFNETMASK, &ifr, error);

        Super::do_log(log_level::debug, log_kind::set_ipv4_netmask,
                      log::str{"netmask", netmask.c_str()});
    }

private:
    auto make_sockaddr(const std::string& ip, std::error_code& error) const
        -> struct sockaddr
    {
        struct sockaddr addr
        {
        };
        struct sockaddr_in* addr_in = (struct sockaddr_in*)&addr;

        if (::inet_aton(ip.c_str(), &addr_in->sin_addr) < 0)
        {
            error = std::error_code(errno, std::generic_category());

            Super::do_log(log_level::error, log_kind::make_sockaddr,
                          log::str{"ip", ip.c_str()},
                          log::str{"error", error.message().c_str()});

            return {};
        }

        addr_in->sin_family = AF_INET;

        Super::do_log(log_level::debug, log_kind::make_sockaddr,
                      log::str{"ip", ip.c_str()});

        return addr;
    }

    auto
    make_ifreq(std::error_code& error) const -> struct ifreq
    {
        struct ifreq ifr
        {
        };

        const std::string& interface_name = Super::interface_name(error);

        if (error)
        {
            return {};
        }

        interface_name.copy(ifr.ifr_name, interface_name.size());

        return ifr;
    }

    private :

        scoped_file_descriptor m_dev_fd;
};
}
}
}
