// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

namespace tunnel
{
namespace detail
{
/// The different kinds of possible log messages
enum class log_kind
{
    initialized,

    /// Creation of an interface
    interface_created,

    /// Renaming of an interface
    interface_renamed,

    /// Setting the owner
    set_owner,

    /// Setting the group
    set_group,

    /// Getting the owner
    owner,

    /// Getting the group
    group,

    /// Getting the interface name
    interface_name,

    /// Getting the native handle
    native_handle,

    /// Checking if the interface is up
    is_up,

    /// Bringing the interface up
    interface_up,

    /// Checking if the interface is down
    is_down,

    /// Bringing the interface down
    interface_down,

    /// Checking if the interface is persistent
    interface_is_persistent,

    /// Setting the interface to be persistent
    set_persistent,

    /// Setting the interface to be non-persistent
    set_non_persistent,

    /// Getting the interface mtu
    interface_mtu,

    /// Setting the interface mtu
    set_mtu,

    /// Checking if the interface is the default route
    is_default_route,

    /// Enabling the interface as the default route
    enable_default_route,

    /// Disabling the interface as the default route
    disable_default_route,

    /// Getting the interface ipv4 address
    interface_ipv4,

    /// Getting the interface ipv4 netmask
    interface_ipv4_netmask,

    /// Setting the interface ipv4 address
    set_ipv4,

    /// Setting the interface ipv4 netmask
    set_ipv4_netmask,

    /// Making the socket address
    make_sockaddr,

    send_netlink,

    recv_netlink_message,

    /// Linux specific
    open,
    socket,
    ioctl,
    send,
    recv,
    bind,
    size,
    read,

    /// Unsupported platform
    unsupported_platform,
};
}
}
