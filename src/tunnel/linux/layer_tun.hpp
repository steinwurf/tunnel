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

#include "error.hpp"
#include "scoped_file_descriptor.hpp"

namespace tunnel
{
namespace linux
{

template <class Super>
struct layer_tun : public Super
{

    void create(const std::string& interface_name, std::error_code& error)
    {
        assert(!error);

        // Check that the interface name is not too long
        // Seems in this patch the name has to be zero teminated so the actual
        // size we can use is IFNAMSIZ - 1:
        // https://www.spinics.net/lists/netdev/msg445126.html
        if (interface_name.size() > IFNAMSIZ - 1)
        {
            error = make_error_code(linux_error::interface_name_too_long);
            return;
        }

        // Open the TUN driver
        const char* tun_device = "/dev/net/tun";
        m_tun_fd = Super::open(tun_device, O_RDWR, error);

        if (error)
        {
            return;
        }

        // Create the TUN interface
        struct ifreq ifr
        {
        };

        ifr.ifr_flags = IFF_TUN;

        if (!interface_name.empty())
        {
            // If a device name was specified, put it in the structure;
            // otherwise, the kernel will try to allocate the "next" device of
            // the specified type
            interface_name.copy(ifr.ifr_name, interface_name.size());
        }

        Super::ioctl(m_tun_fd, TUNSETIFF, (void*)&ifr, error);

        Super::write_log("create interface ", ifr.ifr_name, " error=", error);
    }

    std::string owner(std::error_code& error) const
    {
        assert(!error);

        std::string o = read_property("owner", error);

        Super::write_log("layer_tun: owner=", o, " error=", error);

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

        struct passwd* pwd = getpwnam(owner.c_str());

        if (pwd == nullptr)
        {
            error.assign(errno, std::generic_category());
            return;
        }

        Super::ioctl(m_tun_fd, TUNSETOWNER, (void*)(intptr_t)pwd->pw_uid,
                     error);
    }

    std::string group(std::error_code& error) const
    {
        std::string o = read_property("group", error);

        Super::write_log("layer_tun: group=", o, " error=", error);

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
        assert(m_tun_fd);

        struct group* grp = getgrnam(group.c_str());

        if (grp == nullptr)
        {
            error.assign(errno, std::generic_category());
            return;
        }

        Super::ioctl(m_tun_fd, TUNSETGROUP, (void*)(intptr_t)grp->gr_gid,
                     error);
    }

    bool is_persistent(std::error_code& error) const
    {
        assert(m_tun_fd);
        assert(!error);

        struct ifreq ifr
        {
        };
        Super::ioctl(m_tun_fd, TUNGETIFF, (void*)&ifr, error);

        return ifr.ifr_flags & IFF_PERSIST;
    }

    void set_persistent(std::error_code& error) const
    {
        assert(m_tun_fd);
        assert(!error);

        Super::ioctl(m_tun_fd, TUNSETPERSIST, (void*)1, error);
    }

    void set_non_persistent(std::error_code& error) const
    {
        assert(m_tun_fd);
        assert(!error);

        Super::ioctl(m_tun_fd, TUNSETPERSIST, (void*)0, error);
    }

    std::string interface_name(std::error_code& error) const
    {
        assert(m_tun_fd);
        assert(!error);

        struct ifreq ifr
        {
        };
        Super::ioctl(m_tun_fd, TUNGETIFF, (void*)&ifr, error);

        return ifr.ifr_name;
    }

    int native_handle() const
    {
        assert(m_tun_fd);

        return m_tun_fd.native_handle();
    }

private:
    std::string read_property(const std::string& property,
                              std::error_code& error) const
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

        Super::write_log("layer_tun: property=", ss.str(), " size=", size);

        if (error)
        {
            return {};
        }

        std::vector<uint8_t> data(size, 0);

        uint32_t read = Super::read(fd, data.data(), data.size(), error);

        if (error)
        {
            return {};
        }

        std::string value(data.begin(), data.end());

        Super::write_log("layer_tun: property read=", read, " value=", value);

        return value;
    }

private:
    scoped_file_descriptor m_tun_fd;
};
}
}
