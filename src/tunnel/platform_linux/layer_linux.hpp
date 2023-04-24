// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>

#include "scoped_file_descriptor.hpp"

namespace tunnel
{
namespace platform_linux
{
/// Opens the TUN driver and a socket for configuring the
/// TUN device.
template <class Super>
struct layer_linux : public Super
{
    auto open(const std::string& path, int flags, std::error_code& error) const
        -> scoped_file_descriptor
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

    auto socket(int domain, int type, int protocol,
                std::error_code& error) const -> scoped_file_descriptor
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

    auto send(const scoped_file_descriptor& fd, void* data, uint32_t size,
              int flags, std::error_code& error) const -> uint32_t
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

    auto recv(const scoped_file_descriptor& fd, void* data, uint32_t size,
              int flags, std::error_code& error) const -> uint32_t
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

    auto bind(const scoped_file_descriptor& fd, const struct sockaddr* addr,
              socklen_t addrlen, std::error_code& error) -> uint32_t
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

    auto size(const scoped_file_descriptor& fd, std::error_code& error) const
        -> uint32_t
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

    auto read(const scoped_file_descriptor& fd, void* buf, uint32_t size,
              std::error_code& error) const -> uint32_t
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
