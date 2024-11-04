// Copyright (c) 2024 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <tunnel/interface.hpp>

#include <cassert>
#include <iostream>
#include <platform/config.hpp>

int main()
{
    auto log = [](auto, const std::string& message, auto)
    { std::cout << message << std::endl; };

    tunnel::interface iface;
    iface.set_log_callback(log);
    iface.monitor().enable_log();

#if defined(PLATFORM_LINUX)
    iface.create({tunnel::interface::type::tun, "tuniface"});
    if (iface.is_up())
    {
        iface.set_non_persistent();
        return 0;
    }
#elif defined(PLATFORM_MAC)
    iface.create({});
    if (iface.is_up())
    {
        iface.down();
    }
#endif

#if defined(PLATFORM_LINUX)
    assert(iface.interface_name() == "tuniface");

    assert(iface.is_persistent() == false);
    iface.set_persistent();
    assert(iface.is_persistent() == true);
    iface.set_non_persistent();
#elif defined(PLATFORM_MAC)
    const std::string expected_interface_name =
        "utun"; // on macOS the interface name is not known in advance and must
                // contain "utun" as a substring
    assert(iface.interface_name().find(expected_interface_name) !=
           std::string::npos);
#endif

    iface.set_mtu(1000);
    assert(iface.mtu() == 1000);

    assert(iface.is_down() == true);
    assert(iface.is_up() == false);
    iface.up();
    assert(iface.is_up() == true);
    assert(iface.is_down() == false);
    iface.down();
    assert(iface.is_down() == true);
    assert(iface.is_up() == false);
    iface.up();
    assert(iface.is_up() == true);
    assert(iface.is_down() == false);

    assert(iface.is_default_route() == false);
    iface.enable_default_route();
    assert(iface.is_default_route() == true);
    iface.disable_default_route();
    assert(iface.is_default_route() == false);
    iface.enable_default_route();
    assert(iface.is_default_route() == true);

    iface.set_ipv4("10.0.0.1");
    assert(iface.ipv4() == "10.0.0.1");

    iface.set_ipv4_netmask("255.255.255.0");
    assert(iface.ipv4_netmask() == "255.255.255.0");

    return 0;
}
