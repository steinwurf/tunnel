// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <tunnel/interface.hpp>

#include <cassert>
#include <iostream>
#include <unistd.h>
#include <vector>

/// In this example we read data from the TUN interface
///
/// First run this program:
///
///   ./build/linux/examples/read_data
///
/// Then in a different terminal run e.g.
///
///   ping 10.0.0.2

int main()
{
    auto log = [](auto, const std::string_view& message)
    { std::cout << message << std::endl; };

    tunnel::interface iface;
    iface.monitor().enable_log(log);

    iface.create({});
    iface.set_ipv4("10.0.0.1");
    iface.set_ipv4_netmask("255.255.255.0");
    iface.set_mtu(1500);
    iface.up();

    std::vector<uint8_t> buffer(iface.mtu());

    uint32_t packets = 0;

    while (true)
    {
        int read = ::read(iface.native_handle(), buffer.data(), buffer.size());

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
