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

#include "../../log_level.hpp"

#include "../log.hpp"
#include "../log_kind.hpp"

#include "../scoped_file_descriptor.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_linux
{
/// Opens the TUN driver and a socket for configuring the
/// TUN device.
template <class Super>
struct layer_linux : public Super
{
    auto open(const std::string& path, int flags,
              std::error_code& error) const -> scoped_file_descriptor
    {
        assert(!error);
        assert(!path.empty());

        int fd = ::open(path.c_str(), flags);

        if (fd < 0)
        {
            error = std::error_code(errno, std::generic_category());
            Super::do_log(log_level::error, log_kind::open,
                          log::str{"path", path.c_str()},
                          log::integer{"flags", flags},
                          log::str{"error", error.message().c_str()});
            return scoped_file_descriptor{};
        }

        Super::do_log(log_level::debug, log_kind::open,
                      log::str{"path", path.c_str()},
                      log::integer{"flags", flags}, log::integer{"fd", fd});

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
            Super::do_log(log_level::error, log_kind::socket,
                          log::integer{"domain", domain},
                          log::integer{"type", type},
                          log::integer{"protocol", protocol},
                          log::str{"error", error.message().c_str()});

            return scoped_file_descriptor{};
        }

        Super::do_log(
            log_level::debug, log_kind::socket, log::integer{"domain", domain},
            log::integer{"type", type}, log::integer{"protocol", protocol},
            log::integer{"fd", fd});

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
            Super::do_log(log_level::error, log_kind::ioctl,
                          log::uinteger{"request", request},
                          log::str{"error", error.message().c_str()});
        }

        Super::do_log(log_level::debug, log_kind::ioctl,
                      log::uinteger{"request", request},
                      log::integer{"fd", fd.native_handle()});
    }

    /// Passes an ioctl request that take an int argument (not all request types
    /// takes a pointer to some value) to the TUN driver that we are using or
    /// sets the error_code.
    void ioctl(const scoped_file_descriptor& fd, uint64_t request, int arg,
               std::error_code& error) const
    {
        assert(!error);
        assert(fd);

        if (::ioctl(fd.native_handle(), request, arg) == -1)
        {
            error.assign(errno, std::generic_category());
            Super::do_log(log_level::error, log_kind::ioctl,
                          log::uinteger{"request", request},
                          log::str{"error", error.message().c_str()});
        }

        Super::do_log(log_level::debug, log_kind::ioctl,
                      log::uinteger{"request", request},
                      log::integer{"fd", fd.native_handle()});
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
            Super::do_log(log_level::error, log_kind::send,
                          log::uinteger{"size", size},
                          log::integer{"flags", flags},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        Super::do_log(log_level::debug, log_kind::send,
                      log::uinteger{"size", size}, log::integer{"flags", flags},
                      log::integer{"fd", fd.native_handle()},
                      log::integer{"res", res});

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
            Super::do_log(log_level::error, log_kind::recv,
                          log::uinteger{"size", size},
                          log::integer{"flags", flags},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        Super::do_log(log_level::debug, log_kind::recv,
                      log::uinteger{"size", size}, log::integer{"flags", flags},
                      log::integer{"fd", fd.native_handle()},
                      log::integer{"res", res});

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
            Super::do_log(log_level::error, log_kind::bind,
                          log::uinteger{"addrlen", addrlen},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        Super::do_log(
            log_level::debug, log_kind::bind, log::uinteger{"addrlen", addrlen},
            log::integer{"fd", fd.native_handle()}, log::integer{"res", res});

        return res;
    }

    auto size(const scoped_file_descriptor& fd,
              std::error_code& error) const -> uint32_t
    {
        assert(fd);
        assert(!error);

        off_t current_offset = ::lseek(fd.native_handle(), 0, SEEK_CUR);

        if (current_offset < 0)
        {
            error = std::error_code(errno, std::generic_category());
            Super::do_log(log_level::error, log_kind::size,
                          log::integer{"current_offset", current_offset},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        off_t offset = ::lseek(fd.native_handle(), 0, SEEK_END);

        if (offset < 0)
        {
            error = std::error_code(errno, std::generic_category());
            Super::do_log(log_level::error, log_kind::size,
                          log::integer{"offset", offset},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        off_t set_offset =
            ::lseek(fd.native_handle(), current_offset, SEEK_SET);

        if (set_offset < 0)
        {
            error = std::error_code(errno, std::generic_category());
            Super::do_log(log_level::error, log_kind::size,
                          log::integer{"set_offset", set_offset},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        Super::do_log(log_level::debug, log_kind::size,
                      log::integer{"offset", offset},
                      log::integer{"fd", fd.native_handle()});

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
            Super::do_log(log_level::error, log_kind::read,
                          log::uinteger{"size", size},
                          log::str{"error", error.message().c_str()});
            return 0;
        }

        Super::do_log(
            log_level::debug, log_kind::read, log::uinteger{"size", size},
            log::integer{"fd", fd.native_handle()}, log::integer{"res", res});

        return res;
    }
};
}
}
}
