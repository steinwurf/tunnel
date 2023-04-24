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

// Waf has some problems with this macro which means that
// in the linux/*.hpp files change. If you run
// ./waf build --zone deps you will see that the dependencies
// for this .cpp files does not include the headers if these
// #ifdef are active...
//
// #ifdef PLATFORM_LINUX

#if defined(PLATFORM_LINUX)
#include "platform_linux/tun_interface.hpp"

using platform_tun_interface = tunnel::platform_linux::tun_interface;

#else
#include "platform_unsupported/tun_interface.hpp"

using platform_tun_interface = tunnel::platform_unsupported::tun_interface;

#endif
// #endif

// #ifdef APPLE
// #include "platform_mac/tun_interface.hpp"

// using platform_tun_interface = tunnel::platform_mac::tun_interface;

// #endif

// #ifdef WIN32
// #include "platform_windows/tun_interface.hpp"

// using platform_tun_interface = tunnel::platform_windows::tun_interface;

// #endif

// #else

// #error "Not a supported platform"

// #endif

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
    m_impl->create("", error);
}

void tun_interface::create(const std::string& device_name)
{
    assert(m_impl);

    std::error_code error;
    m_impl->create(device_name, error);
    throw_if_error(error);
}
void tun_interface::create(const std::string& device_name,
                           std::error_code& error)
{
    assert(m_impl);
    m_impl->create(device_name, error);
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

std::string tun_interface::owner() const
{
    assert(m_impl);

    std::error_code error;
    std::string own = owner(error);
    throw_if_error(error);
    return own;
}
std::string tun_interface::owner(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->owner(error);
}

std::string tun_interface::group() const
{
    assert(m_impl);

    std::error_code error;
    std::string grp = group(error);
    throw_if_error(error);
    return grp;
}
std::string tun_interface::group(std::error_code& error) const
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

std::string tun_interface::interface_name() const
{
    assert(m_impl);

    std::error_code error;
    std::string name = interface_name(error);
    throw_if_error(error);
    return name;
}

std::string tun_interface::interface_name(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->interface_name(error);
}

bool tun_interface::is_persistent(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->is_persistent(error);
}

bool tun_interface::is_persistent() const
{
    assert(m_impl);

    std::error_code error;
    bool persistent = m_impl->is_persistent(error);
    throw_if_error(error);
    return persistent;
}

bool tun_interface::is_up(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->is_up(error);
}

bool tun_interface::is_up() const
{
    assert(m_impl);

    std::error_code error;
    bool up = m_impl->is_up(error);
    throw_if_error(error);
    return up;
}

bool tun_interface::is_down(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->is_down(error);
}

bool tun_interface::is_down() const
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

uint32_t tun_interface::mtu() const
{
    assert(m_impl);

    std::error_code error;
    uint32_t mtu = m_impl->mtu(error);
    throw_if_error(error);
    return mtu;
}
uint32_t tun_interface::mtu(std::error_code& error) const
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

bool tun_interface::is_default_route() const
{
    assert(m_impl);

    std::error_code error;
    bool is_default = is_default_route(error);
    throw_if_error(error);
    return is_default;
}
bool tun_interface::is_default_route(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->is_default_route(error);
}

std::string tun_interface::ipv4() const
{
    assert(m_impl);

    std::error_code error;
    std::string ip = ipv4(error);
    throw_if_error(error);
    return ip;
}

std::string tun_interface::ipv4(std::error_code& error) const
{
    assert(m_impl);
    return m_impl->ipv4(error);
}

std::string tun_interface::ipv4_netmask() const
{
    assert(m_impl);

    std::error_code error;
    std::string ip = ipv4_netmask(error);
    throw_if_error(error);
    return ip;
}

std::string tun_interface::ipv4_netmask(std::error_code& error) const
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

void tun_interface::disable_log_stdout()
{
    assert(m_impl);
    m_impl->disable_log_stdout();
}

void tun_interface::enable_log_stdout()
{
    assert(m_impl);
    m_impl->enable_log_stdout();
}

bool tun_interface::is_log_enabled() const
{
    assert(m_impl);
    return m_impl->is_log_enabled();
}

int tun_interface::native_handle() const
{
    assert(m_impl);
    return m_impl->native_handle();
}

auto tun_interface::is_platform_supported() -> bool
{
    return platform_tun_interface::is_platform_supported();
}

}
