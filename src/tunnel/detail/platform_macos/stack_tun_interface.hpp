// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "../layer_final.hpp"
#include "../layer_monitor.hpp"
#include "layer_interface.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_macos
{
/// In Linux you can find the documentation for TUN/TAP devices
/// here: https://www.kernel.org/doc/Documentation/networking/tuntap.txt

// clang-format off
struct stack_tun_interface : public
    layer_interface<
    layer_monitor<
    layer_final<stack_tun_interface>>>>>>
{
    static auto is_platform_supported() -> bool
    {
        return true;
    }

    static auto type() -> std::string
    {
        return "tunnel::detail::platform_mac::stack_tun_interface";
    }
};
// clang-format on
}
}
}
