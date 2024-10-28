// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "../../interface_config.hpp"
#include "../monitor.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_unsupported
{
struct unsupported
{
    unsupported() : m_monitor("unsupported", {})
    {
    }

    static bool is_platform_supported()
    {
        return false;
    }

    void create(const config&, std::error_code& error)
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void rename(const std::string&, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_owner(const std::string&, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string&, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string interface_name(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    bool is_up(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    bool is_down(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void up(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void down(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    int mtu(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return 0;
    }

    void set_non_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_mtu(int, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_ipv4(const std::string&, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void enable_default_route(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void disable_default_route(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_default_route(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    std::string ipv4(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string ipv4_netmask(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_ipv4_netmask(const std::string&, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    int native_handle() const
    {
        throw std::runtime_error("not supported");
        return -1;
    }
    auto monitor() const -> const tunnel::monitor&
    {
        return m_monitor;
    }

    auto monitor() -> tunnel::monitor&
    {
        return m_monitor;
    }

    auto set_log_callback(const log_callback&) -> void
    {
    }
    tunnel::detail::monitor m_monitor;
};

}
}
}
