// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>
#include <sstream>
#include <system_error>
#include <vector>

#include <fcntl.h>
#include <grp.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../../interface.hpp"
#include "../../log_level.hpp"
#include "../action.hpp"
#include "../log.hpp"
#include "../scoped_file_descriptor.hpp"
#include "error.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_linux
{

template <class Super>
struct layer_interface : public Super
{

    void create(const tunnel::interface::config& config, std::error_code& error)
    {
        assert(!error);

        // Check that the interface name is not too long
        // Seems in this patch the name has to be zero-terminated so the actual
        // size we can use is IFNAMSIZ - 1:
        // https://www.spinics.net/lists/netdev/msg445126.html
        if (config.interface_name.size() > IFNAMSIZ - 1)
        {
            error = make_error_code(linux_error::interface_name_too_long);

            Super::do_log(log_level::error, action::interface_created,
                          log::str{"error", error.message().c_str()});
            assert(error);
            return;
        }

        // Open the TUN driver
        m_interface_fd = Super::open("/dev/net/tun", O_RDWR, error);

        if (error)
        {
            return;
        }

        // Create the TUN interface
        struct ifreq ifr
        {
        };
        switch (config.interface_type)
        {
        case tunnel::interface::type::tun:
        {
            ifr.ifr_flags = IFF_TUN;
            break;
        }
        case tunnel::interface::type::tap:
        {
            ifr.ifr_flags = IFF_TAP;
            break;
        }
        default:
        {
            error = make_error_code(linux_error::invalid_interface_type);
            return;
        }
        }

        // Do not prepend a protocol information header.
        //
        // *Without* the IFF_NO_PI flag, the driver will send the following;
        // two bytes of flags, two bytes of protocol type, and then the actual
        // network packet.
        // Since the first two values are largely redundant, most applications
        // will probably want to set this flag, hence we do so here.
        if (config.iff_no_pi)
        {
            ifr.ifr_flags |= IFF_NO_PI;
        }

        // If the vnet_hdr flag is set, we want to enable the virtio-net-header
        // feature. This feature is used to pass additional information about
        // GSO packets to and from the kernel.
        if (config.vnet_hdr)
        {
            ifr.ifr_flags |= IFF_VNET_HDR;
        }

        // If a device name was specified, put it in the structure;
        // otherwise, the kernel will try to allocate the "next" device of
        // the specified type
        if (!config.interface_name.empty())
        {
            config.interface_name.copy(ifr.ifr_name,
                                       config.interface_name.size());
        }

        Super::ioctl(m_interface_fd, TUNSETIFF, (void*)&ifr, error);
        if (error)
        {
            return;
        }

        if (config.vnet_hdr)
        {
            if (config.interface_type == tunnel::interface::type::tun)
            {
                int offload_flags = TUN_F_CSUM | TUN_F_TSO4 | TUN_F_UFO;
                Super::ioctl(m_interface_fd, TUNSETOFFLOAD, offload_flags,
                             error);
                if (error)
                {
                    return;
                }
            }
            else if (config.interface_type == tunnel::interface::type::tap)
            {
                // there are some support for offloading but it is not clear how
                // and if they work
            }
        }

        Super::do_log(log_level::debug, action::interface_created,
                      log::str{"name", config.interface_name.c_str()},
                      log::boolean{"vnet_hdr", config.vnet_hdr});
    }

    auto owner(std::error_code& error) const -> std::string
    {
        assert(!error);

        std::string o = read_property("owner", error);

        Super::do_log(log_level::debug, action::owner,
                      log::str{"owner", o.c_str()});

        if (error)
        {
            return {};
        }

        int uid = stoi(o, nullptr);

        if (uid == -1)
        {
            // https://unix.stackexchange.com/a/278488
            return {};
        }

        struct passwd* pwd = getpwuid(uid);

        if (pwd == nullptr)
        {
            error.assign(errno, std::generic_category());
            return {};
        }

        return pwd->pw_name;
    }

    void set_owner(const std::string& owner, std::error_code& error) const
    {
        assert(!error);
        assert(!owner.empty());

        Super::do_log(log_level::debug, action::set_owner,
                      log::str{"owner", owner.c_str()});

        struct passwd* pwd = getpwnam(owner.c_str());

        if (pwd == nullptr)
        {
            error.assign(errno, std::generic_category());
            return;
        }

        Super::ioctl(m_interface_fd, TUNSETOWNER, (void*)(intptr_t)pwd->pw_uid,
                     error);
    }

    auto group(std::error_code& error) const -> std::string
    {
        std::string o = read_property("group", error);

        Super::do_log(log_level::debug, action::group,
                      log::str{"group", o.c_str()});

        if (error)
        {
            return {};
        }

        int uid = stoi(o, nullptr);

        if (uid == -1)
        {
            // https://unix.stackexchange.com/a/278488
            return {};
        }

        struct group* grp = getgrgid(uid);

        if (grp == nullptr)
        {
            error.assign(errno, std::generic_category());
            return {};
        }

        return grp->gr_name;
    }

    void set_group(const std::string& group, std::error_code& error) const
    {
        assert(!error);
        assert(!group.empty());
        assert(m_interface_fd);

        Super::do_log(log_level::debug, action::set_group,
                      log::str{"group", group.c_str()});

        struct group* grp = getgrnam(group.c_str());

        if (grp == nullptr)
        {
            error.assign(errno, std::generic_category());
            return;
        }

        Super::ioctl(m_interface_fd, TUNSETGROUP, (void*)(intptr_t)grp->gr_gid,
                     error);
    }

    auto is_persistent(std::error_code& error) const -> bool
    {
        assert(m_interface_fd);
        assert(!error);

        struct ifreq ifr
        {
        };
        Super::ioctl(m_interface_fd, TUNGETIFF, (void*)&ifr, error);

        bool persistent = ifr.ifr_flags & IFF_PERSIST;

        Super::do_log(log_level::debug, action::interface_is_persistent,
                      log::boolean{"persistent", persistent});
        return persistent;
    }

    void set_persistent(std::error_code& error) const
    {
        assert(m_interface_fd);
        assert(!error);

        Super::do_log(log_level::debug, action::set_persistent,
                      log::str{"", ""});

        Super::ioctl(m_interface_fd, TUNSETPERSIST, (void*)1, error);
    }

    void set_non_persistent(std::error_code& error) const
    {
        assert(m_interface_fd);
        assert(!error);

        Super::do_log(log_level::debug, action::set_non_persistent,
                      log::str{"", ""});

        Super::ioctl(m_interface_fd, TUNSETPERSIST, (void*)0, error);
    }

    auto interface_name(std::error_code& error) const -> std::string
    {
        assert(m_interface_fd);
        assert(!error);

        struct ifreq ifr
        {
        };
        Super::ioctl(m_interface_fd, TUNGETIFF, (void*)&ifr, error);

        Super::do_log(log_level::debug, action::interface_name,
                      log::str{"name", ifr.ifr_name});

        return ifr.ifr_name;
    }

    auto native_handle() const -> int
    {
        assert(m_interface_fd);

        Super::do_log(log_level::debug, action::native_handle,
                      log::integer{"handle", m_interface_fd.native_handle()});

        return m_interface_fd.native_handle();
    }

private:
    auto read_property(const std::string& property,
                       std::error_code& error) const -> std::string
    {
        std::string name = interface_name(error);

        if (error)
        {
            return {};
        }

        // https://github.com/sivasankariit/iproute2/blob/master/ip/iptuntap.c#L217
        std::stringstream ss;
        ss << "/sys/class/net/" << name << "/" << property;

        scoped_file_descriptor fd = Super::open(ss.str(), O_RDONLY, error);

        if (error)
        {
            return {};
        }

        uint32_t size = Super::size(fd, error);

        if (error)
        {
            return {};
        }

        std::vector<uint8_t> data(size, 0);

        Super::read(fd, data.data(), data.size(), error);

        if (error)
        {
            return {};
        }

        std::string value(data.begin(), data.end());

        return value;
    }

private:
    scoped_file_descriptor m_interface_fd;
};
}
}
}
