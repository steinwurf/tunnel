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

const uint32_t max_buffer_size = 65600;
bool verbose = false;

void udp_receive_handler(tunnel::tun_interface& tun,
                         boost::asio::ip::udp::socket& socket,
                         std::vector<uint8_t>& rx_buffer,
                         const boost::system::error_code& ec,
                         std::size_t bytes)
{
    if (ec && ec != boost::system::errc::operation_canceled)
    {
        std::cout << "Error on udp receive: " << ec.message() << std::endl;
        exit(ec.value());
    }

    if (ec == boost::system::errc::operation_canceled)
    {
        return;
    }

    rx_buffer.resize(bytes);

    if (verbose) // verbose is global variable
    {
        std::cout << "Received buffer of size " << bytes << " bytes on udp"
                  << std::endl;
    }
    // Write buffer to tun interface
    std::error_code error;
    tun.write(rx_buffer.data(), rx_buffer.size(), error);
    if (error)
    {
        std::cout << "Error on tun interface write: " << error.message()
                  << std::endl;
        exit(error.value());
    }

    // reset buffer size before next receive call
    rx_buffer.resize(max_buffer_size);

    // Qeuue another receive call
    socket.async_receive(
        boost::asio::buffer(rx_buffer),
        std::bind(&udp_receive_handler,
                  std::ref(tun),
                  std::ref(socket),
                  std::ref(rx_buffer),
                  std::placeholders::_1,
                  std::placeholders::_2));
}

void tun_read_handler(tunnel::tun_interface& tun,
                      boost::asio::ip::udp::socket& socket,
                      std::vector<uint8_t>& tx_buffer,
                      boost::asio::ip::udp::endpoint remote,
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

    if (verbose) // verbose is global variable
    {
        std::cout << "Sending buffer of size " << bytes << " bytes on udp"
                  << std::endl;
    }
    // Send buffer to udp socket
    boost::system::error_code error;
    socket.send_to(boost::asio::buffer(tx_buffer), remote, 0, error);
    if (error)
    {
        std::cout << "Error on udp socket send: " << error.message()
                  << std::endl;
        exit(error.value());
    }

    // reset buffer size before next receive call
    tx_buffer.resize(max_buffer_size);

    // Qeuue another read call
    tun.async_read(tx_buffer.data(), tx_buffer.size(),
                   std::bind(&tun_read_handler,
                             std::ref(tun),
                             std::ref(socket),
                             std::ref(tx_buffer),
                             remote,
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
    bool route;

    // Parse the prorgram options
    bpo::options_description options("Commandline Options");

    options.add_options()
    ("tunnel_ip,t", bpo::value<std::string>(&tunnel_ip)->required(),
     "Specify the IPv4 address to set on the created tunnel interface [required]")
    ("local_ip,l", bpo::value<std::string>(&local_ip)->required(),
     "Specify the IPv4 address of the local interface that the tunnel should send to and receive from [required]")
    ("remote_ip,r", bpo::value<std::string>(&remote_ip)->required(),
     "Specify the remote IPv4 address the tunnel should send to and receive from [required]")
    ("port,p", bpo::value<uint16_t>(&port)->default_value(9999),
     "Set the port to use for the udp tunnel")
    ("name,n", bpo::value<std::string>(&tunnel_name)->default_value("tunwurf"),
     "Set the tunnel interface name")
    ("default_route,d", bpo::bool_switch(&route)->default_value(false),
     "Use this flag if the tunnel should be default route")
    // 'verbose' is global namespace variable
    ("verbose,v", bpo::bool_switch(&verbose)->default_value(false),
     "Use this flag for verbose output")
    ("help,h", "Print this help message");

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
    std::cout << "Setting up virtual interface with ip " << tunnel_ip
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

    std::cout << "Virtual interface \"" << tun->device_name() << "\" created."
              << std::endl;

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
    std::cout << "ip address set to " << tun->ipv4(error) << std::endl;

    // Set MTU
    tun->set_mtu(25000, error);
    if (error)
    {
        std::cout << "Error setting tunnel interface MTU: " << error.message()
                  << std::endl;
        return error.value();
    }

    // Set default route through tunnel interface if specified
    if (route)
    {
        std::cout << "Setting up default route through tunnel interface."
                  << std::endl;

        tun->enable_default_route(error);
        if (error)
        {
            std::cout << "Error setting default route to tunnel interface: "
                      << error.message() << std::endl;
            return error.value();
        }
    }

    std::vector<uint8_t> buffer(max_buffer_size);

    auto local_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::from_string(local_ip),
        port);

    boost::system::error_code ec;
    boost::asio::ip::udp::socket udp_socket(io);
    udp_socket.open(local_endpoint.protocol(), ec);
    if (ec)
    {
        std::cout << "Error opening socket: " << ec.message() << std::endl;
        return ec.value();
    }

    // bind to specified address
    udp_socket.bind({local_endpoint.address(), port}, ec);
    if (ec)
    {
        std::cout << "Error binding socket: " << ec.message() << std::endl;
        return ec.value();
    }

    // allow socket to transmit broadcast packets
    boost::asio::socket_base::broadcast opt(true);
    udp_socket.set_option(opt, ec);
    if (ec)
    {
        std::cout << "Error enabling broadcast: " << ec.message() << std::endl;
        return ec.value();
    }

    uint32_t max_buffer_size = 66000;
    std::vector<uint8_t> rx_buffer(max_buffer_size);
    std::vector<uint8_t> tx_buffer(max_buffer_size);

    // Setup receive->write functionality
    // UDP packets received should be forwarded to tun write
    udp_socket.async_receive(
        boost::asio::buffer(rx_buffer),
        std::bind(&udp_receive_handler,
                  std::ref(*tun),
                  std::ref(udp_socket),
                  std::ref(rx_buffer),
                  std::placeholders::_1,
                  std::placeholders::_2));

    auto remote_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::from_string(remote_ip),
        port);
    // Setup read->send functionality
    // Packets read from tun should be forwarded to UDP send
    tun->async_read(tx_buffer.data(), tx_buffer.size(),
                    std::bind(&tun_read_handler,
                              std::ref(*tun),
                              std::ref(udp_socket),
                              std::ref(tx_buffer),
                              remote_endpoint,
                              std::placeholders::_1,
                              std::placeholders::_2));

    io.run();

    udp_socket.close(ec);
    if (ec)
    {
        std::cout << "Error closing udp socket: " << ec.message() << std::endl;
        return ec.value();
    }
    // remove default route through tunnel interface
    if (route)
    {
        tun->disable_default_route(error);
        if (error)
        {
            std::cout << "Error removing default route from tunnel interface: "
                      << error.message() << std::endl;
            return error.value();
        }
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
