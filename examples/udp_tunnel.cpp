// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/tun_interface.hpp>

#include <iostream>
#include <vector>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <links/udp/link.hpp>

const uint32_t max_buffer_size = 65600;

void udp_receive_handler(tunnel::tun_interface& tun,
                         links::udp::link& link,
                         std::vector<uint8_t>& rx_buffer,
                         const std::error_code& ec,
                         uint32_t bytes)
{
    if (ec && ec != std::errc::operation_canceled)
    {
        std::cout << "Error on udp receive: " << ec.message() << std::endl;
        exit(ec.value());
    }

    if (ec == std::errc::operation_canceled)
    {
        return;
    }

    rx_buffer.resize(bytes);

    std::cout << "Received buffer of size " << bytes << " bytes on udp"
              << std::endl;

    // Write buffer to tun interface
    std::error_code error;
    tun.write(rx_buffer, error);
    if (error)
    {
        std::cout << "Error on tun interface write: " << error.message()
                  << std::endl;
        exit(error.value());
    }

    // reset buffer size before next receive call
    rx_buffer.resize(max_buffer_size);

    // Qeuue another receive call
    link.async_receive(rx_buffer,
                       std::bind(&udp_receive_handler,
                                 std::ref(tun),
                                 std::ref(link),
                                 std::ref(rx_buffer),
                                 std::placeholders::_1,
                                 std::placeholders::_2));
}

void tun_read_handler(tunnel::tun_interface& tun,
                      links::udp::link& link,
                      std::vector<uint8_t>& tx_buffer,
                      const std::error_code& ec,
                      uint32_t bytes)
{
    if (ec && ec != std::errc::operation_canceled)
    {
        std::cout << "Error on udp receive: " << ec.message() << std::endl;
        exit(ec.value());
    }

    if (ec == std::errc::operation_canceled)
    {
        return;
    }

    tx_buffer.resize(bytes);

    std::cout << "Sending buffer of size " << bytes << " bytes on udp" << std::endl;

    // Send buffer to udp link
    std::error_code error;
    link.send(tx_buffer, error);
    if (error)
    {
        std::cout << "Error on udp link send: " << error.message() << std::endl;
        exit(error.value());
    }

    // reset buffer size before next receive call
    tx_buffer.resize(max_buffer_size);

    // Qeuue another read call
    tun.async_read(tx_buffer,
                   std::bind(&tun_read_handler,
                             std::ref(tun),
                             std::ref(link),
                             std::ref(tx_buffer),
                             std::placeholders::_1,
                             std::placeholders::_2));
}

int main(int argc, char* argv[])
{
    namespace bpo = boost::program_options;

    std::string local_ip;
    std::string remote_ip;
    uint16_t port;
    std::string tunnel_ip;
    std::string tunnel_name;

    // Parse the prorgram options
    bpo::options_description options("Commandline Options");

    options.add_options()(
        "local_ip,l",
        bpo::value<std::string>(&local_ip)->required(),
        "Specify the local IPv4 address to use for the tunnel [required]")(
        "remote_ip,r",
        bpo::value<std::string>(&remote_ip)->required(),
        "Specify the remote IPv4 address to use for the tunnel [required]")(
        "tunnel_ip,t",
        bpo::value<std::string>(&tunnel_ip)->required(),
        "Specify the IPv4 address to set on the tunnel interface [required]")(
        "port,p",
        bpo::value<uint16_t>(&port)->default_value(0xbeef),
        "Set the port to use for the udp tunnel")(
        "name,n",
        bpo::value<std::string>(&tunnel_name)->default_value("tunwurf"),
        "Set the tunnel interface name")("help,h", "Print this help message");

    bpo::variables_map opts;

    // Verify all required options are present
    try
    {
        bpo::store(bpo::parse_command_line(argc, argv, options), opts);

        if (opts.count("help"))
        {
            std::cout << options << std::endl;
            return 0;
        }

        bpo::notify(opts);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error when parsing commandline options: " << e.what()
                  << std::endl;
        std::cout << "See list of options with \"" << argv[0] << " --help\""
                  << std::endl;
        return 0;
    }

    // Print results of argument parse
    std::cout << "Setting up udp tunnel between endpoints " << local_ip << ":"
              << port << " (local) and " << remote_ip << ":" << port
              << " (remote)." << std::endl;
    std::cout << "Setting up virtual interface \"" << tunnel_name
              << "\" with ip " << tunnel_ip
              << ". All communication on this interface will go through the "
                 "udp tunnel."
              << std::endl;

    boost::asio::io_service io;
    std::error_code error;

    auto tun =
        tunnel::tun_interface::make_tun_interface(io, tunnel_name, error);
    if (error)
    {
        std::cout << "Error creating tunnel interface: " << error.message()
                  << std::endl;
        return error.value();
    }

    // Set interface up
    tun->up(error);
    if (error)
    {
        std::cout << "Error setting tunnel interface up: " << error.message()
                  << std::endl;
        return error.value();
    }

    // "Set IP address"
    tun->set_ipv4(tunnel_ip, error);
    if (error)
    {
        std::cout << "Error setting ipv4 on tunnel interface: "
                  << error.message() << std::endl;
        return error.value();
    }

    std::cout << "Setting up default route through tunnel interface."
              << std::endl;
    // Set default route through tunnel interface
    tun->set_default_route(error);
    if (error)
    {
        std::cout << "Error setting default route to tunnel interface: "
                  << error.message() << std::endl;
        return error.value();
    }


    std::vector<uint8_t> buffer(max_buffer_size);


    // Setup UDP tunnel
    links::udp::link udp_link(io);

    udp_link.bind(
        boost::asio::ip::address_v4::from_string(local_ip), port, error);
    if (error)
    {
        std::cout << "Error binding to address: " << error.message()
                  << std::endl;
        return error.value();
    }

    udp_link.add_remote(
        boost::asio::ip::address_v4::from_string(remote_ip), port, error);
    if (error)
    {
        std::cout << "Error adding remote address: " << error.message()
                  << std::endl;
        return error.value();
    }

    uint32_t max_buffer_size = 66000;
    std::vector<uint8_t> udp_rx_buffer(max_buffer_size);
    std::vector<uint8_t> udp_tx_buffer(max_buffer_size);

    // Setup receive->write functionality
    // UDP packets received should be forwarded to tun write
    //
    udp_link.async_receive(udp_rx_buffer,
                           std::bind(&udp_receive_handler,
                                     std::ref(*tun),
                                     std::ref(udp_link),
                                     std::ref(udp_rx_buffer),
                                     std::placeholders::_1,
                                     std::placeholders::_2));

    // Setup read->send functionality
    // Packets read from tun should be forwarded to UDP send

    tun->async_read(udp_tx_buffer,
                    std::bind(&tun_read_handler,
                              std::ref(*tun),
                              std::ref(udp_link),
                              std::ref(udp_tx_buffer),
                              std::placeholders::_1,
                              std::placeholders::_2));

    io.run();

    udp_link.close();

    // remove default route through tunnel interface
    tun->remove_default_route(error);
    if (error)
    {
        std::cout << "Error removing default route from tunnel interface: "
        << error.message() << std::endl;
        return error.value();
    }

    tun->down(error);
    if (error)
    {
        std::cout << "Error setting tunnel interface down: " << error.message()
                  << std::endl;
        return error.value();
    }

    return 0;
}
