// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <cassert>
#include <cstring>
#include <string>
#include <system_error>

#include <netinet/in.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <linux/if_tun.h>

// clang-format off
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
// clang-format on

#include "error.hpp"
#include "layer_linux.hpp"

#include "layer_netdevice.hpp"
#include "layer_netlink_v4.hpp"
#include "layer_tun.hpp"
#include "scoped_file_descriptor.hpp"

#include "../detail/layer_final.hpp"
#include "../detail/layer_monitor.hpp"

namespace tunnel
{
namespace platform_linux
{
/// In Linux you can find the documentation for TUN/TAP devices
/// here: https://www.kernel.org/doc/Documentation/networking/tuntap.txt

// clang-format off
struct tun_interface : public
    layer_netlink_v4<
    layer_netdevice<
    layer_tun<
    layer_linux<
    tunnel::detail::layer_monitor<
    tunnel::detail::layer_final<tun_interface>>>>>>
{
    static auto is_platform_supported() -> bool
    {
        return true;
    }

    static auto type() -> std::string
    {
        return "tunnel::platform_linux::tun_interface";
    }
};
// clang-format on
}
}
