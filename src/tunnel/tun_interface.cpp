// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

// clang-format off
#include "throw_if_error.hpp"
#include "tun_interface.hpp"
// clang-format on

#include <cassert>

#include <platform/config.hpp>

#if defined(PLATFORM_LINUX)

#include "detail/platform_linux/stack_tun_interface.hpp"
using platform_tun_interface =
    tunnel::detail::platform_linux::stack_tun_interface;

#else

#include "detail/platform_unsupported/stack_tun_interface.hpp"
using platform_tun_interface =
    tunnel::detail::platform_unsupported::stack_tun_interface;

#endif

namespace tunnel
{

struct tun_interface::impl : platform_tun_interface
{
};

tun_interface::tun_interface()
{
    m_impl = std::make_unique<tun_interface::impl>();
}

tun_interface::tun_interface(tun_interface&& iface)
{
    m_impl = std::move(iface.m_impl);
}

tun_interface& tun_interface::operator=(tun_interface&& iface)
{
    m_impl = std::move(iface.m_impl);
    return *this;
}

tun_interface::~tun_interface()
{
}

void tun_interface::create()
{
    assert(m_impl);

    std::error_code error;
    create(error);
    throw_if_error(error);
}
void tun_interface::create(std::error_code& error)
{
    assert(m_impl);
    create("", false, error);
}

void tun_interface::create(const std::string& device_name,
                           std::error_code& error)
{
    assert(m_impl);
    create(device_name, false, error);
}

void tun_interface::create(const std::string& device_name, bool vnet_hdr)
{
    assert(m_impl);

    std::error_code error;
    create(device_name, vnet_hdr, error);
    throw_if_error(error);
}

void tun_interface::create(const std::string& device_name, bool vnet_hdr,
                           std::error_code& error)
{
    assert(m_impl);
    m_impl->create(device_name, vnet_hdr, error);
}

void tun_interface::rename(const std::string& interface_name) const
{
    assert(m_impl);

    std::error_code error;
    rename(interface_name, error);
    throw_if_error(error);
}

void tun_interface::rename(const std::string& interface_name,
                           std::error_code& error) const
{
    assert(m_impl);
    m_impl->rename(interface_name, error);
}

auto tun_interface::owner() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string own = owner(error);
    throw_if_error(error);
    return own;
}
auto tun_interface::owner(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->owner(error);
}

auto tun_interface::group() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string grp = group(error);
    throw_if_error(error);
    return grp;
}
auto tun_interface::group(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->group(error);
}

void tun_interface::set_owner(const std::string& owner) const
{
    assert(m_impl);

    std::error_code error;
    set_owner(owner, error);
    throw_if_error(error);
}
void tun_interface::set_owner(const std::string& owner,
                              std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_owner(owner, error);
}

void tun_interface::set_group(const std::string& group) const
{
    assert(m_impl);

    std::error_code error;
    set_group(group, error);
    throw_if_error(error);
}
void tun_interface::set_group(const std::string& group,
                              std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_group(group, error);
}

auto tun_interface::interface_name() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string name = interface_name(error);
    throw_if_error(error);
    return name;
}

auto tun_interface::interface_name(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->interface_name(error);
}

auto tun_interface::is_persistent(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_persistent(error);
}

auto tun_interface::is_persistent() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool persistent = m_impl->is_persistent(error);
    throw_if_error(error);
    return persistent;
}

auto tun_interface::is_up(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_up(error);
}

auto tun_interface::is_up() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool up = m_impl->is_up(error);
    throw_if_error(error);
    return up;
}

auto tun_interface::is_down(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_down(error);
}

auto tun_interface::is_down() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool down = m_impl->is_down(error);
    throw_if_error(error);
    return down;
}

void tun_interface::up() const
{
    assert(m_impl);

    std::error_code error;
    up(error);
    throw_if_error(error);
}
void tun_interface::up(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->up(error);
}

void tun_interface::down() const
{
    assert(m_impl);

    std::error_code error;
    down(error);
    throw_if_error(error);
}
void tun_interface::down(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->down(error);
}

void tun_interface::set_persistent(std::error_code& error)
{
    assert(m_impl);
    m_impl->set_persistent(error);
}
void tun_interface::set_non_persistent(std::error_code& error)
{
    assert(m_impl);
    m_impl->set_non_persistent(error);
}

void tun_interface::set_persistent()
{
    assert(m_impl);

    std::error_code error;
    m_impl->set_persistent(error);
    throw_if_error(error);
}
void tun_interface::set_non_persistent()
{
    assert(m_impl);

    std::error_code error;
    m_impl->set_non_persistent(error);
    throw_if_error(error);
}

auto tun_interface::mtu() const -> uint32_t
{
    assert(m_impl);

    std::error_code error;
    uint32_t mtu = m_impl->mtu(error);
    throw_if_error(error);
    return mtu;
}

auto tun_interface::mtu(std::error_code& error) const -> uint32_t
{
    assert(m_impl);
    return m_impl->mtu(error);
}

void tun_interface::set_mtu(uint32_t mtu) const
{
    assert(m_impl);

    std::error_code error;
    set_mtu(mtu, error);
    throw_if_error(error);
}
void tun_interface::set_mtu(uint32_t mtu, std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_mtu(mtu, error);
}

void tun_interface::enable_default_route() const
{
    assert(m_impl);

    std::error_code error;
    enable_default_route(error);
    throw_if_error(error);
}
void tun_interface::enable_default_route(std::error_code& error) const
{
    assert(m_impl);
    m_impl->enable_default_route(error);
}

void tun_interface::disable_default_route() const
{
    assert(m_impl);

    std::error_code error;
    disable_default_route(error);
    throw_if_error(error);
}
void tun_interface::disable_default_route(std::error_code& error) const
{
    assert(m_impl);
    m_impl->disable_default_route(error);
}

auto tun_interface::is_default_route() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool is_default = is_default_route(error);
    throw_if_error(error);
    return is_default;
}

auto tun_interface::is_default_route(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_default_route(error);
}

auto tun_interface::ipv4() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string ip = ipv4(error);
    throw_if_error(error);
    return ip;
}

auto tun_interface::ipv4(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->ipv4(error);
}

auto tun_interface::ipv4_netmask() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string ip = ipv4_netmask(error);
    throw_if_error(error);
    return ip;
}

auto tun_interface::ipv4_netmask(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->ipv4_netmask(error);
}

void tun_interface::set_ipv4(const std::string& ip) const
{
    assert(m_impl);

    std::error_code error;
    set_ipv4(ip, error);
    throw_if_error(error);
}
void tun_interface::set_ipv4(const std::string& ip,
                             std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_ipv4(ip, error);
}

void tun_interface::set_ipv4_netmask(const std::string& mask) const
{
    assert(m_impl);

    std::error_code error;
    set_ipv4_netmask(mask, error);
    throw_if_error(error);
}

void tun_interface::set_ipv4_netmask(const std::string& mask,
                                     std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_ipv4_netmask(mask, error);
}

auto tun_interface::native_handle() const -> int
{
    assert(m_impl);
    return m_impl->native_handle();
}

auto tun_interface::monitor() const -> const tunnel::monitor&
{
    assert(m_impl);
    return m_impl->monitor();
}

auto tun_interface::monitor() -> tunnel::monitor&
{
    assert(m_impl);
    return m_impl->monitor();
}

auto tun_interface::set_log_callback(const log_callback& callback) -> void
{
    assert(m_impl);
    m_impl->set_log_callback(callback);
}

auto tun_interface::is_platform_supported() -> bool
{
    return platform_tun_interface::is_platform_supported();
}

}
