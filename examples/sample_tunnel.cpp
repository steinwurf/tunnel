// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <CLI/CLI.hpp>
#include <asio.hpp>
#include <tunnel/tun_interface.hpp>

#include <cassert>
#include <deque>
#include <iostream>
#include <unistd.h>
#include <vector>

/**
 * Create Tunnel:
 * Local machine (192.168.1.2):
 * sudo build/linux/examples/udp_tunnel2 --tunnel_ip 10.0.0.1 --remote_ip
 * 192.168.1.3 --local_ip 192.168.1.2
 *
 * Remote machine (192.168.1.3):
 * sudo build/linux/examples/udp_tunnel2 --tunnel_ip 10.0.0.2 --remote_ip
 * 192.168.1.2 --local_ip 192.168.1.3
 *
 * Communication:
 * Local machine (192.168.1.2 / 10.0.0.1):
 * ping 10.0.0.2
 *
 * This will cause data to go from:
 *
 *   10.0.0.1 --tun--> 192.168.1.2 --network--> 10.0.0.2 --tun--> 192.168.1.3
 */
class sample_tunnel
{
public:
    sample_tunnel(asio::io_service& io, int tun_fd, uint32_t mtu,
                  asio::ip::udp::endpoint local_endpoint,
                  asio::ip::udp::endpoint remote_endpoint) :
        m_socket(io),
        m_mtu(mtu), m_stream_descriptor(io), m_local_endpoint(local_endpoint),
        m_remote_endpoint(remote_endpoint)
    {
        m_stream_descriptor.assign(tun_fd);
    }

    void start()
    {
        m_socket.open(m_local_endpoint.protocol());

        // bind to specified address
        m_socket.bind(m_local_endpoint);

        // allow the socket to transmit broadcast packets
        m_socket.set_option(asio::socket_base::broadcast(true));

        async_network_receive();
        async_tun_read();
    }

    void stop()
    {
        m_socket.close();
        m_stream_descriptor.cancel();
        m_stream_descriptor.close();
    }

private:
    void async_network_receive()
    {
        m_rx_buffer.resize(m_mtu);
        m_socket.async_receive(
            asio::buffer(m_rx_buffer),
            std::bind(&sample_tunnel::handle_async_network_receive, this,
                      std::placeholders::_1, std::placeholders::_2));
    }

    void handle_async_network_receive(const asio::error_code& ec,
                                      std::size_t bytes)
    {
        if (ec == asio::error::operation_aborted)
        {
            return;
        }
        else if (ec)
        {
            std::cout << "Error on network receive: " << ec.message()
                      << std::endl;
            exit(ec.value());
        }
        std::cout << "udp receive " << bytes << std::endl;
        m_rx_buffer.resize(bytes);
        asio::write(m_stream_descriptor, asio::buffer(m_rx_buffer));
        async_network_receive();
    }

    void async_tun_read()
    {
        m_tx_buffer.resize(m_mtu);
        m_stream_descriptor.async_read_some(
            asio::buffer(m_tx_buffer),
            std::bind(&sample_tunnel::handle_async_tun_read, this,
                      std::placeholders::_1, std::placeholders::_2));
    }

    void handle_async_tun_read(const asio::error_code& ec, std::size_t bytes)
    {
        if (ec == asio::error::operation_aborted)
        {
            return;
        }
        else if (ec)
        {
            std::cout << "Error on reading tun: " << ec.message() << std::endl;
            exit(ec.value());
        }
        std::cout << "tun read " << bytes << std::endl;
        m_tx_buffer.resize(bytes);
        m_socket.send_to(asio::buffer(m_tx_buffer), m_remote_endpoint);
        async_tun_read();
    }

private:
    asio::ip::udp::socket m_socket;

    const uint32_t m_mtu;

    // The tun file descriptor stream descriptor
    asio::posix::stream_descriptor m_stream_descriptor;

    const asio::ip::udp::endpoint m_local_endpoint;
    const asio::ip::udp::endpoint m_remote_endpoint;

    std::vector<uint8_t> m_rx_buffer;
    std::vector<uint8_t> m_tx_buffer;
};

int main(int argc, char* argv[])
{
    std::string local_ip;
    std::string remote_ip;
    uint16_t port;
    std::string tunnel_ip;

    CLI::App app{"Sample Tunnel"};

    app.add_option("-t,--tunnel_ip", tunnel_ip,
                   "Specify the IPv4 address to set on the created tunnel "
                   "interface")
        ->required();
    app.add_option("-l,--local_ip", local_ip,
                   "Specify the IPv4 address of the local interface that the "
                   "tunnel should send to and receive from")
        ->required();
    app.add_option("-r,--remote_ip", remote_ip,
                   "Specify the remote IPv4 address the tunnel should send to "
                   "and receive from")
        ->required();
    app.add_option("-p,--port", port, "Set the port to use for the UDP tunnel")
        ->default_val(9999);

    CLI11_PARSE(app, argc, argv);

    // Print results of argument parse
    std::cout << "Setting up udp tunnel between endpoints " << local_ip << ":"
              << port << " (local) and " << remote_ip << ":" << port
              << " (remote)." << std::endl;
    std::cout << "Setting up the virtual interface with ip " << tunnel_ip
              << ". All communication on this interface will go through the "
                 "udp tunnel."
              << std::endl;

    tunnel::tun_interface iface;

    iface.create();
    iface.up();

    iface.set_ipv4(tunnel_ip);
    iface.set_ipv4_netmask("255.255.255.0");

    asio::io_service io;

    auto local_endpoint = asio::ip::udp::endpoint(
        asio::ip::address_v4::from_string(local_ip), port);

    auto remote_endpoint = asio::ip::udp::endpoint(
        asio::ip::address_v4::from_string(remote_ip), port);

    sample_tunnel tunnel(io, iface.native_handle(), iface.mtu(), local_endpoint,
                         remote_endpoint);

    tunnel.start();

    io.run();

    return 0;
}
