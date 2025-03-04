// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <system_error>

#include <netinet/in.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <linux/if_tun.h>

#include <linux/if.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "error.hpp"
#include "layer_linux.hpp"

#include "../scoped_file_descriptor.hpp"
#include "layer_interface.hpp"
#include "layer_netdevice.hpp"
#include "layer_netlink_v4.hpp"

#include "../action.hpp"
#include "layer_final.hpp"
#include "layer_monitor.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_linux
{
/// In Linux you can find the documentation for TUN/TAP devices
/// here: https://www.kernel.org/doc/Documentation/networking/tuntap.txt

// clang-format off
struct stack_interface : public
    layer_netlink_v4<
    layer_netdevice<
    layer_interface<
    layer_linux<
    layer_monitor<
    layer_final<stack_interface>>>>>>
{
    static auto name() -> std::string
    {
        return "linux_tunnel_interface";
    }

    static auto actions() -> std::vector<poke::action>
    {
        return action::poke_registry;
    }
};
// clang-format on
}
}
}
