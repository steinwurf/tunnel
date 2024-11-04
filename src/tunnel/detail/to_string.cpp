// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "to_string.hpp"
#include "log_kind.hpp"

namespace tunnel
{
namespace detail
{

auto to_string(log_kind kind) -> std::string
{
    switch (kind)
    {
    case log_kind::initialized:
        return "initialized";
    case log_kind::interface_created:
        return "interface_created";
    case log_kind::interface_renamed:
        return "interface_renamed";
    case log_kind::set_owner:
        return "set_owner";
    case log_kind::set_group:
        return "set_group";
    case log_kind::owner:
        return "owner";
    case log_kind::group:
        return "group";
    case log_kind::interface_name:
        return "interface_name";
    case log_kind::native_handle:
        return "native_handle";
    case log_kind::is_up:
        return "is_up";
    case log_kind::interface_up:
        return "interface_up";
    case log_kind::is_down:
        return "is_down";
    case log_kind::interface_down:
        return "interface_down";
    case log_kind::interface_is_persistent:
        return "interface_is_persistent";
    case log_kind::set_persistent:
        return "set_persistent";
    case log_kind::set_non_persistent:
        return "set_non_persistent";
    case log_kind::interface_mtu:
        return "interface_mtu";
    case log_kind::set_mtu:
        return "set_mtu";
    case log_kind::is_default_route:
        return "is_default_route";
    case log_kind::enable_default_route:
        return "enable_default_route";
    case log_kind::disable_default_route:
        return "disable_default_route";
    case log_kind::ipv4:
        return "ipv4";
    case log_kind::ipv4_netmask:
        return "ipv4_netmask";
    case log_kind::set_ipv4:
        return "set_ipv4";
    case log_kind::set_ipv4_netmask:
        return "set_ipv4_netmask";
    case log_kind::make_sockaddr:
        return "make_sockaddr";
    case log_kind::send_netlink:
        return "send_netlink";
    case log_kind::recv_netlink_message:
        return "recv_netlink_message";
    case log_kind::open:
        return "open";
    case log_kind::socket:
        return "socket";
    case log_kind::ioctl:
        return "ioctl";
    case log_kind::send:
        return "send";
    case log_kind::recv:
        return "recv";
    case log_kind::bind:
        return "bind";
    case log_kind::size:
        return "size";
    case log_kind::read:
        return "read";
    case log_kind::unsupported_platform:
        return "unsupported_platform";
    default:
        return "unknown";
    }
}
}

}
