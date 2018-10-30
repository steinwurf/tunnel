// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>

#include "scoped_file_descriptor.hpp"

namespace tunnel
{
namespace linux
{
/// Opens the TUN driver and a socket for configuring the
/// TUN device.
template <class Super>
struct layer_linux : public Super
{
    scoped_file_descriptor open(const std::string& path, int flags,
                                std::error_code& error) const
    {
        assert(!error);
        assert(!path.empty());

        int fd = ::open(path.c_str(), flags);

        if (fd < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return scoped_file_descriptor{};
        }

        return scoped_file_descriptor{fd};
    }

    scoped_file_descriptor socket(int domain, int type, int protocol,
                                  std::error_code& error) const
    {
        assert(!error);

        int fd = ::socket(domain, type, protocol);

        if (fd < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return scoped_file_descriptor{};
        }

        return scoped_file_descriptor{fd};
    }

    /// Passes an ioctl request to the TUN driver that we are using or
    /// sets the error_code.
    void ioctl(const scoped_file_descriptor& fd, uint64_t request, void* arg,
               std::error_code& error) const
    {
        assert(!error);
        assert(fd);

        if (::ioctl(fd.native_handle(), request, arg) == -1)
        {
            error.assign(errno, std::generic_category());
        }
    }

    uint32_t send(const scoped_file_descriptor& fd, void* data, uint32_t size,
                  int flags, std::error_code& error) const
    {
        assert(!error);
        assert(fd);
        assert(data);
        assert(size > 0);

        int res = ::send(fd.native_handle(), data, size, flags);

        if (res < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        return res;
    }

    uint32_t recv(const scoped_file_descriptor& fd, void* data, uint32_t size,
                  int flags, std::error_code& error) const
    {
        assert(!error);
        assert(fd);
        assert(data);
        assert(size > 0);

        int res = ::recv(fd.native_handle(), data, size, flags);

        if (res < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        return res;
    }

    int bind(const scoped_file_descriptor& fd, const struct sockaddr* addr,
             socklen_t addrlen, std::error_code& error)
    {
        assert(!error);
        assert(fd);
        assert(addr);
        assert(addrlen > 0);

        int res = ::bind(fd.native_handle(), addr, addrlen);

        if (res < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        return res;
    }

    uint32_t size(const scoped_file_descriptor& fd,
                  std::error_code& error) const
    {
        assert(fd);
        assert(!error);

        off_t current_offset = ::lseek(fd.native_handle(), 0, SEEK_CUR);

        if (current_offset < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        off_t offset = ::lseek(fd.native_handle(), 0, SEEK_END);

        if (offset < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        off_t set_offset =
            ::lseek(fd.native_handle(), current_offset, SEEK_SET);

        if (set_offset < 0)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        return offset;
    }

    uint32_t read(const scoped_file_descriptor& fd, void* buf, uint32_t size,
                  std::error_code& error) const
    {
        assert(fd);
        assert(buf);
        assert(size > 0);
        assert(!error);

        ssize_t res = ::read(fd.native_handle(), buf, size);

        if (res == -1)
        {
            error = std::error_code(errno, std::generic_category());
            return 0;
        }

        return res;
    }
};
}
}
