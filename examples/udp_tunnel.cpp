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

#include <links/udp/link.hpp>

int main()
{
    std::string localaddr = "10.0.0.1";
    std::string remoteaddr = "10.0.0.2";
    uint16_t port = 0xbeef;

    std::string tunaddr = "10.0.1.1";

    std::string name("tunwurf");
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

    // "Set IP address"
    tun->set_ipv4(tunaddr, error);
    if (error)
    {
        std::cout << "Error setting ipv4 on tun interface: " << error.message()
                  << std::endl;
        return error.value();
    }

    // Setup UDP tunnel
    links::udp::link udp_link(io);

    udp_link.bind(
        boost::asio::ip::address_v4::from_string(localaddr), port, error);
    if (error)
    {
        std::cout << "Error binding to address: " << error.message()
                  << std::endl;
        return error.value();
    }

    udp_link.add_remote(
        boost::asio::ip::address_v4::from_string(remoteaddr), port, error);
    if (error)
    {
        std::cout << "Error adding remote address: " << error.message()
                  << std::endl;
        return error.value();
    }


    // Setup receive->write functionality
    // UDP packets received should be forwarded to tun write

    DO STUFF HERE


    // Setup read->send functionality
    // Packets read from tun should be forwarded to UDP send

    DO STUFF HERE AS WELL

    // // Setup a single async read that in turn will do a write of the read buffer
    // tun->async_read(buffer, [&](auto error, auto bytes) {
    //     if (error && error != std::errc::operation_canceled)
    //     {
    //         std::cout << "Error on async read: " << error.message()
    //                   << std::endl;
    //     }
    //     else if (error != std::errc::operation_canceled)
    //     {
    //         std::cout << "Read a packet of " << bytes << " bytes"
    //                   << " from the interface " << std::endl;
    //     }
    //
    //     buffer.resize(bytes);
    //
    //     tun->async_write(buffer, [&](auto error, auto bytes) {
    //         if (error && error != std::errc::operation_canceled)
    //         {
    //             std::cout << "Error on async send: " << error.message()
    //                       << std::endl;
    //         }
    //         else if (error != std::errc::operation_canceled)
    //         {
    //             std::cout << "Wrote a packet of " << bytes << " bytes"
    //                       << " to the interface " << std::endl;
    //         }
    //
    //         io.stop();
    //     });
    // });


    io.run();

    udp_link.close();

    tun->down(error);
    if (error)
    {
        std::cout << "Error setting tun interface down: " << error.message()
                  << std::endl;
        return error.value();
    }

    return 0;
}
