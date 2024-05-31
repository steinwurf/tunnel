// Copyright (c) 2024 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <string>
#include <system_error>

#include "../../log_level.hpp"
#include "../log_kind.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_unsupported
{
template <class Super>
class layer_tap : public Super
{
public:
    void create(const std::string& interface_name, bool vnet_hdr,
                std::error_code& error)
    {
        (void)interface_name;
        (void)vnet_hdr;
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void rename(const std::string& interface_name, std::error_code& error) const
    {
        (void)interface_name;

        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_owner(const std::string& owner, std::error_code& error) const
    {
        (void)owner;

        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string& group, std::error_code& error) const
    {
        (void)group;
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string interface_name(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    bool is_up(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    bool is_down(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void up(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void down(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    int mtu(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return 0;
    }

    void set_non_persistent(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_mtu(int mtu, std::error_code& error) const
    {
        (void)mtu;
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_ipv4(const std::string& address, std::error_code& error) const
    {
        (void)address;
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void enable_default_route(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void disable_default_route(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_default_route(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    std::string ipv4(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string ipv4_netmask(std::error_code& error) const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_ipv4_netmask(const std::string& netmask,
                          std::error_code& error) const
    {
        (void)netmask;

        Super::do_log(log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    int native_handle() const
    {
        Super::do_log(log_level::error, log_kind::unsupported_platform);
        throw std::runtime_error("not supported");
        return -1;
    }
};
}
}
}
