// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <tunnel/tun_interface.hpp>

#include <cassert>
#include <iostream>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

/// In this example we read data from the TUN interface
///
/// First run this program:
///
///   ./build/linux/examples/backtobacktun
///
/// Then in a different terminal run e.g.
///
///   ping 10.0.0.2

int main()
{
    auto log1 = [](const std::string& message)
    { std::cout << "iface1: " << message << std::endl; };

    auto log2 = [](const std::string& message)
    { std::cout << "iface2: " << message << std::endl; };

    tunnel::tun_interface iface1;
    tunnel::tun_interface iface2;
    iface1.monitor().enable_log(log1);
    iface2.monitor().enable_log(log2);

    iface1.create();
    iface1.set_ipv4("10.0.0.1");
    iface1.set_ipv4_netmask("255.255.255.0");
    iface1.set_mtu(1500);
    iface1.up();

    int error = ::ioctl(iface1.native_handle(), TUNSETOFFLOAD, 1);

    if (error < 0)
    {
        std::cout << "Error setting offload" << std::endl;
        exit(1);
    }

    // iface2.create();
    // iface2.set_ipv4("10.0.0.2");
    // iface2.set_ipv4_netmask("255.255.255.0");
    // iface2.set_mtu(1500);
    // iface2.up();

    std::vector<uint8_t> buffer(1000000);

    uint32_t packets = 0;

    while (true)
    {
        int read = ::read(iface1.native_handle(), buffer.data(), buffer.size());

        if (read < 0)
        {
            std::cout << "Error reading from TUN" << std::endl;
            exit(1);
        }

        std::cout << packets << ": Read " << read << " bytes" << std::endl;
        ++packets;
    }

    return 0;
}
