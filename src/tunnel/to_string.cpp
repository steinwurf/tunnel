// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "to_string.hpp"

namespace tunnel
{

auto to_string(log_kind kind) -> std::string
{
    switch (kind)
    {
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
    case log_kind::interface_ipv4:
        return "interface_ipv4_address";
    case log_kind::set_ipv4:
        return "set_ipv4_address";
    case log_kind::interface_ipv4_netmask:
        return "interface_ipv4_netmask";
    case log_kind::set_ipv4_netmask:
        return "set_ipv4_netmask";
    case log_kind::unsupported_platform:
        return "unsupported_platform";
    default:
        return "unknown";
    }
}

}
