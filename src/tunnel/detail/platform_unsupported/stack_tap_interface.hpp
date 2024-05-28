// Copyright (c) 2024 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include "layer_tap.hpp"

#include "../layer_final.hpp"
#include "../layer_monitor.hpp"
namespace tunnel
{
namespace detail
{
namespace platform_unsupported
{
// clang-format off
struct stack_tun_interface : public
    layer_tap<
    layer_monitor<
    layer_final<stack_tun_interface>>>
// clang-format on
{
    static bool is_platform_supported()
    {
        return false;
    }

    static auto type() -> std::string
    {
        return "tunnel::detail::platform_unsupported::stack_tap_interface";
    }
};

}
}
}
