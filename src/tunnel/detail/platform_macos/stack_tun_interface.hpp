#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <system_error>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/sys_domain.h>

#include "platform_layer.hpp"
#include "error_handling_layer.hpp"
#include "resource_management_layer.hpp"
#include "configuration_layer.hpp"
#include "logging_monitoring_layer.hpp"
#include "persistence_layer.hpp"
#include "interface_control_layer.hpp"

#include "../layer_final.hpp"
#include "../layer_monitor.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_macos
{
// clang-format off
struct stack_tun_interface : public
    platform_layer<
    error_handling_layer<
    resource_management_layer<
    configuration_layer<
    logging_monitoring_layer<
    persistence_layer<
    interface_control_layer<
    layer_monitor<
    layer_final<stack_tun_interface>>>>>>>>>
{
    static auto is_platform_supported() -> bool
    {
        return true;
    }

    static auto type() -> std::string
    {
        return "tunnel::detail::platform_macos::stack_utun_interface";
    }
};
// clang-format on
}
}
}
