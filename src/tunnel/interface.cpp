// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <cassert>

#include "interface.hpp"
#include "throw_if_error.hpp"

#include <platform/config.hpp>

#if defined(PLATFORM_LINUX)
<<<<<<< HEAD:src/tunnel/tun_interface.cpp

#include "detail/platform_linux/stack_tun_interface.hpp"
using platform_tun_interface =
    tunnel::detail::platform_linux::stack_tun_interface;

#elif defined(PLATFORM_MAC)

#include "detail/platform_macos/tun_interface.hpp"
using platform_tun_interface = tunnel::detail::platform_macos::tun_interface;

=======
#include "detail/platform_linux/stack_interface.hpp"
using platform_interface = tunnel::detail::platform_linux::stack_interface;
static constexpr bool platform_supported = true;
#elif defined(PLATFORM_MAC)
#include "detail/platform_macos/interface.hpp"
using platform_interface = tunnel::detail::platform_macos::interface;
static constexpr bool platform_supported = true;
>>>>>>> master:src/tunnel/interface.cpp
#else
#include "detail/platform_unsupported/unsupported.hpp"
using platform_interface = tunnel::detail::platform_unsupported::unsupported;
static constexpr bool platform_supported = false;
#endif

namespace tunnel
{

struct interface::impl : platform_interface
{
};

interface::interface()
{
    m_impl = std::make_unique<interface::impl>();
}

interface::interface(interface&& iface)
{
    m_impl = std::move(iface.m_impl);
}

interface& interface::operator=(interface&& iface)
{
    m_impl = std::move(iface.m_impl);
    return *this;
}

interface::~interface()
{
}

void interface::create(const interface::config& config)
{
    assert(m_impl);

    std::error_code error;
    create(config, error);
    throw_if_error(error);
}

void interface::create(const interface::config& config, std::error_code& error)
{
    assert(m_impl);
    m_impl->create(config, error);
}

void interface::rename(const std::string& interface_name) const
{
    assert(m_impl);

    std::error_code error;
    rename(interface_name, error);
    throw_if_error(error);
}

void interface::rename(const std::string& interface_name,
                       std::error_code& error) const
{
    assert(m_impl);
    m_impl->rename(interface_name, error);
}

auto interface::owner() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string own = owner(error);
    throw_if_error(error);
    return own;
}
auto interface::owner(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->owner(error);
}

auto interface::group() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string grp = group(error);
    throw_if_error(error);
    return grp;
}
auto interface::group(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->group(error);
}

void interface::set_owner(const std::string& owner) const
{
    assert(m_impl);

    std::error_code error;
    set_owner(owner, error);
    throw_if_error(error);
}
void interface::set_owner(const std::string& owner,
                          std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_owner(owner, error);
}

void interface::set_group(const std::string& group) const
{
    assert(m_impl);

    std::error_code error;
    set_group(group, error);
    throw_if_error(error);
}
void interface::set_group(const std::string& group,
                          std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_group(group, error);
}

auto interface::interface_name() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string name = interface_name(error);
    throw_if_error(error);
    return name;
}

auto interface::interface_name(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->interface_name(error);
}

auto interface::is_persistent(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_persistent(error);
}

auto interface::is_persistent() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool persistent = m_impl->is_persistent(error);
    throw_if_error(error);
    return persistent;
}

auto interface::is_up(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_up(error);
}

auto interface::is_up() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool up = m_impl->is_up(error);
    throw_if_error(error);
    return up;
}

auto interface::is_down(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_down(error);
}

auto interface::is_down() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool down = m_impl->is_down(error);
    throw_if_error(error);
    return down;
}

void interface::up() const
{
    assert(m_impl);

    std::error_code error;
    up(error);
    throw_if_error(error);
}
void interface::up(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->up(error);
}

void interface::down() const
{
    assert(m_impl);

    std::error_code error;
    down(error);
    throw_if_error(error);
}
void interface::down(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->down(error);
}

void interface::set_persistent(std::error_code& error)
{
    assert(m_impl);
    m_impl->set_persistent(error);
}
void interface::set_non_persistent(std::error_code& error)
{
    assert(m_impl);
    m_impl->set_non_persistent(error);
}

void interface::set_persistent()
{
    assert(m_impl);

    std::error_code error;
    m_impl->set_persistent(error);
    throw_if_error(error);
}
void interface::set_non_persistent()
{
    assert(m_impl);

    std::error_code error;
    m_impl->set_non_persistent(error);
    throw_if_error(error);
}

auto interface::mtu() const -> uint32_t
{
    assert(m_impl);

    std::error_code error;
    uint32_t mtu = m_impl->mtu(error);
    throw_if_error(error);
    return mtu;
}

auto interface::mtu(std::error_code& error) const -> uint32_t
{
    assert(m_impl);
    return m_impl->mtu(error);
}

void interface::set_mtu(uint32_t mtu) const
{
    assert(m_impl);

    std::error_code error;
    set_mtu(mtu, error);
    throw_if_error(error);
}
void interface::set_mtu(uint32_t mtu, std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_mtu(mtu, error);
}

void interface::enable_default_route() const
{
    assert(m_impl);

    std::error_code error;
    enable_default_route(error);
    throw_if_error(error);
}
void interface::enable_default_route(std::error_code& error) const
{
    assert(m_impl);
    m_impl->enable_default_route(error);
}

void interface::disable_default_route() const
{
    assert(m_impl);

    std::error_code error;
    disable_default_route(error);
    throw_if_error(error);
}
void interface::disable_default_route(std::error_code& error) const
{
    assert(m_impl);
    m_impl->disable_default_route(error);
}

auto interface::is_default_route() const -> bool
{
    assert(m_impl);

    std::error_code error;
    bool is_default = is_default_route(error);
    throw_if_error(error);
    return is_default;
}

auto interface::is_default_route(std::error_code& error) const -> bool
{
    assert(m_impl);
    return m_impl->is_default_route(error);
}

auto interface::ipv4() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string ip = ipv4(error);
    throw_if_error(error);
    return ip;
}

auto interface::ipv4(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->ipv4(error);
}

auto interface::ipv4_netmask() const -> std::string
{
    assert(m_impl);

    std::error_code error;
    std::string ip = ipv4_netmask(error);
    throw_if_error(error);
    return ip;
}

auto interface::ipv4_netmask(std::error_code& error) const -> std::string
{
    assert(m_impl);
    return m_impl->ipv4_netmask(error);
}

void interface::set_ipv4(const std::string& ip) const
{
    assert(m_impl);

    std::error_code error;
    set_ipv4(ip, error);
    throw_if_error(error);
}
void interface::set_ipv4(const std::string& ip, std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_ipv4(ip, error);
}

void interface::set_ipv4_netmask(const std::string& mask) const
{
    assert(m_impl);

    std::error_code error;
    set_ipv4_netmask(mask, error);
    throw_if_error(error);
}

void interface::set_ipv4_netmask(const std::string& mask,
                                 std::error_code& error) const
{
    assert(m_impl);
    m_impl->set_ipv4_netmask(mask, error);
}

auto interface::native_handle() const -> int
{
    assert(m_impl);
    return m_impl->native_handle();
}

auto interface::monitor() const -> const tunnel::monitor&
{
    assert(m_impl);
    return m_impl->monitor();
}

auto interface::monitor() -> tunnel::monitor&
{
    assert(m_impl);
    return m_impl->monitor();
}

auto interface::is_platform_supported() -> bool
{
    return platform_supported;
}

}
