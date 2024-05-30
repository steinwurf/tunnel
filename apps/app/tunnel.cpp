// Copyright (c) 2024 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.
#include <CLI/CLI.hpp>
#include <asio.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <system_error>
#include <tunnel/tap_interface.hpp>
#include <tunnel/tun_interface.hpp>
#if defined(PLATFORM_LINUX)
#include <sys/resource.h>
inline void enable_core_dumps()
{
    // core dumps may be disallowed by the parent of this process; change that
    struct rlimit core_limits;
    core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &core_limits);
}
#else
inline void enable_core_dumps()
{
    // do nothing
}
#endif

inline auto parse_ip(const std::string& address) -> asio::ip::address
{
    // If there is a port
    std::size_t found = address.rfind(':');

    if (found != std::string::npos)
    {

        // Make address and check if it is valid
        asio::error_code ec;
        asio::ip::address addr =
            asio::ip::make_address(address.substr(0, found), ec);
        if (ec)
        {
            throw std::runtime_error("Invalid address: " + address);
        }
        return addr;
    }
    else
    {
        asio::error_code ec;
        asio::ip::address addr = asio::ip::make_address(address, ec);
        if (ec)
        {
            throw std::runtime_error("Invalid address: " + address);
        }
        return addr;
    }
}

/// Parse port from string, while ignoring the ip.
/// 10.0.0.1      -> error::invalid_argument
/// 10.0.0.1:9900 -> 9900
inline auto parse_port(const std::string& address) -> uint16_t
{
    // If there is a port
    std::size_t found = address.rfind(':');
    if (found != std::string::npos)
    {
        auto port = address.substr(found + 1, std::string::npos);
        return std::atoi(port.c_str());
    }
    else
    {
        return 0;
    }
}
inline auto to_udp_endpoint(const std::string& address)
    -> asio::ip::udp::endpoint
{
    asio::ip::address addr = parse_ip(address);
    uint16_t port = parse_port(address);
    return {addr, port};
}
asio::ip::udp::endpoint peer;
auto rx_udp_tx_tun(asio::ip::udp::socket& rx,
                   asio::posix::stream_descriptor& tx) -> void
{
    uint8_t buffer[2000];
    rx.async_receive_from(asio::buffer(buffer, sizeof(buffer)), peer,
                          [&](const std::error_code&, std::size_t len)
                          {
                              tx.write_some(asio::buffer(buffer, len));
                              rx_udp_tx_tun(rx, tx);
                          });
}
auto rx_tun_tx_udp(asio::posix::stream_descriptor& rx,
                   asio::ip::udp::socket& tx) -> void
{
    uint8_t buffer[2000];
    rx.async_read_some(asio::buffer(buffer, sizeof(buffer)),
                       [&](const std::error_code&, std::size_t len)
                       {
                           if (peer != asio::ip::udp::endpoint())
                           {
                               tx.send_to(asio::buffer(buffer, len), peer);
                           }
                           rx_tun_tx_udp(rx, tx);
                       });
}

int main(int argc, char** argv)
{
    enable_core_dumps();

    CLI::App app{"Tunnel back-to-back test app"};

    std::string mode = "";
    std::string tunnel_address = "";
    std::string local_address = "";
    std::string remote_address = "";

    app.add_option("-m", mode, "Tunnel mode tun/tap")->required();
    app.add_option("-a", tunnel_address, "Tunnel address")->required();
    app.add_option("-l", local_address, "UDP local address");
    app.add_option("-r", remote_address, "UDP remote address");
    auto log1 = [](auto, const std::string& message, auto)
    { std::cout << "tunnel_iface: " << message << std::endl; };

    CLI11_PARSE(app, argc, argv);
    assert((!local_address.empty() || !remote_address.empty()) &&
           "Either remote or local address must be specified");
    assert(!tunnel_address.empty() && "Empty tunnel address");
    asio::io_context io;
    asio::ip::udp::socket udp_socket(io, asio::ip::udp::v4());
    if (!local_address.empty())
    {
        auto ep = to_udp_endpoint(local_address);
        udp_socket.bind(ep);
    }
    else if (!remote_address.empty())
    {
        auto ep = to_udp_endpoint(remote_address);
        peer = ep;
    }

    if (mode == "tun")
    {
        tunnel::tun_interface iface1;
        iface1.set_log_callback(log1);
        iface1.monitor().enable_log();
        iface1.create();
        iface1.set_ipv4(tunnel_address);
        iface1.set_ipv4_netmask("255.255.255.0");
        iface1.set_mtu(1500);
        iface1.up();
        auto in_fd = iface1.native_handle();
        assert(in_fd > 0 && "Invalid file descriptor");
        auto rx = asio::posix::stream_descriptor(io);
        rx.assign(in_fd);
        rx_tun_tx_udp(rx, udp_socket);
        rx_udp_tx_tun(udp_socket, rx);
        io.run();
    }
    else if (mode == "tap")
    {
        tunnel::tap_interface iface1;
        iface1.set_log_callback(log1);
        iface1.monitor().enable_log();
        iface1.create();
        iface1.set_ipv4(tunnel_address);
        iface1.set_ipv4_netmask("255.255.255.0");
        iface1.set_mtu(1500);
        iface1.up();
        auto in_fd = iface1.native_handle();
        assert(in_fd > 0 && "Invalid file descriptor");
        auto rx = asio::posix::stream_descriptor(io);
        rx.assign(in_fd);
        rx_tun_tx_udp(rx, udp_socket);
        rx_udp_tx_tun(udp_socket, rx);
        io.run();
    }

    return 0;
}
