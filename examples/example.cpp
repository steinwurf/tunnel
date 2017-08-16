// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/tun_interface.hpp>

#include <iostream>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

int main()
{
    std::string name("testtun");
    boost::asio::io_service io;
    std::error_code error;

    auto tun = tunnel::tun_interface::make_tun_interface(io, name, error);
    if (error)
    {
        std::cout << "Error creating tun interface: " << error.message()
                  << std::endl;
        return error.value();
    }

    // Set interface up
    tun->up(error);
    if (error)
    {
        std::cout << "Error setting tun interface up: " << error.message()
                  << std::endl;
        return error.value();
    }

    uint32_t max_buffer_size = 65600;
    std::vector<uint8_t> buffer(max_buffer_size);

    // Setup a single async read that in turn will do a write of the read buffer
    tun->async_read(buffer, [&](auto error, auto bytes) {
        if (error && error != std::errc::operation_canceled)
        {
            std::cout << "Error on async read: " << error.message()
                      << std::endl;
        }
        else if (error != std::errc::operation_canceled)
        {
            std::cout << "Read a packet of " << bytes << " bytes"
                      << " from the interface " << std::endl;
        }

        buffer.resize(bytes);

        tun->async_write(buffer, [&](auto error, auto bytes) {
            if (error && error != std::errc::operation_canceled)
            {
                std::cout << "Error on async send: " << error.message()
                          << std::endl;
            }
            else if (error != std::errc::operation_canceled)
            {
                std::cout << "Wrote a packet of " << bytes << " bytes"
                          << " to the interface " << std::endl;
            }

            io.stop();
        });
    });

    // "Set IP address"
    tun->set_ipv4("10.0.0.2", error);
    if (error)
    {
        std::cout << "Error setting ipv4 on tun interface: " << error.message()
                  << std::endl;
        return error.value();
    }

    io.run();

    tun->down(error);
    if (error)
    {
        std::cout << "Error setting tun interface down: " << error.message()
                  << std::endl;
        return error.value();
    }

    return 0;
}
