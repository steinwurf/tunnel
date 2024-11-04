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
#include <tunnel/interface.hpp>

#if defined(PLATFORM_LINUX)
#include <sys/resource.h>
void enable_core_dumps()
{
    // core dumps may be disallowed by the parent of this process; change that
    struct rlimit core_limits;
    core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &core_limits);
}
#else
void enable_core_dumps()
{
    // do nothing
}
#endif

auto to_endpoint(const std::string& endpoint_str) -> asio::ip::udp::endpoint
{
    std::size_t found = endpoint_str.rfind(':');
    if (found == std::string::npos)
    {
        throw std::runtime_error("Invalid endpoint: " + endpoint_str);
    }

    asio::error_code ec;
    asio::ip::address addr =
        asio::ip::make_address(endpoint_str.substr(0, found), ec);
    if (ec)
    {
        throw std::runtime_error("Invalid address: " + endpoint_str);
    }

    uint16_t port = std::atoi(endpoint_str.substr(found + 1).c_str());

    return {addr, port};
}

auto rx_udp_tx_tun(asio::ip::udp::socket& rx,
                   asio::posix::stream_descriptor& tx,
                   asio::ip::udp::endpoint& peer) -> void
{
    static uint8_t buffer[2000];
    rx.async_receive_from(asio::buffer(buffer, sizeof(buffer)), peer,
                          [&](const std::error_code& err, std::size_t len)
                          {
                              assert(!err);
                              tx.write_some(asio::buffer(buffer, len));
                              rx_udp_tx_tun(rx, tx, peer);
                          });
}
auto rx_tun_tx_udp(asio::posix::stream_descriptor& rx,
                   asio::ip::udp::socket& tx,
                   const asio::ip::udp::endpoint& peer) -> void
{
    static uint8_t buffer[2000];
    rx.async_read_some(asio::buffer(buffer, sizeof(buffer)),
                       [&](const std::error_code&, std::size_t len)
                       {
                           if (peer != asio::ip::udp::endpoint())
                           {
                               tx.send_to(asio::buffer(buffer, len), peer);
                           }
                           rx_tun_tx_udp(rx, tx, peer);
                       });
}

struct EndpointValidator : public CLI::Validator
{
    EndpointValidator()
    {
        name_ = "ENDPOINT";
        func_ = [](const std::string& str) -> std::string
        {
            try
            {
                to_endpoint(str);
            }
            catch (const std::exception& e)
            {
                return e.what();
            }
            return std::string();
        };
    }
};

int main(int argc, char** argv)
{
    enable_core_dumps();

    std::string mode = "";
    std::string tunnel_address = "";
    std::string local_address = "";
    std::string remote_address = "";

    CLI::App app{"Tunnel back-to-back test app"};

    app.add_option("-m,--mode", mode, "Tunnel mode tun/tap")
        ->check(CLI::IsMember({"tun",
#if defined(PLATFORM_LINUX) // Tap is only supported on Linux
                               "tap"
#endif
        }))
        ->required();
    app.add_option("-a,--tunnel", tunnel_address,
                   "Tunnel address, e.g. 11.11.11.11")
        ->check(CLI::ValidIPV4)
        ->required();

    auto local_option =
        app.add_option("-l,--local", local_address, "UDP local endpoint")
            ->check(EndpointValidator());
    auto remote_option =
        app.add_option("-r,--remote", remote_address, "UDP remote endpoint")
            ->check(EndpointValidator());

    // Make sure that local and remote address are mutually exclusive
    local_option->excludes(remote_option);
    remote_option->excludes(local_option);

    // Add a validation callback to enforce that either local or remote address
    // is specified
    app.final_callback(
        [&]()
        {
            if (local_address.empty() && remote_address.empty())
            {
                throw CLI::RequiredError("Either --local or --remote");
            }
        });

    CLI11_PARSE(app, argc, argv);
    asio::io_context io;
    asio::ip::udp::socket udp_socket(io, asio::ip::udp::v4());
    asio::ip::udp::endpoint peer;
    if (!local_address.empty())
    {
        udp_socket.bind(to_endpoint(local_address));
    }
    else if (!remote_address.empty())
    {
        peer = to_endpoint(remote_address);
    }

    auto log1 = [](auto, const std::string& message, auto)
    { std::cout << "tunnel_iface: " << message << std::endl; };
    tunnel::interface::config config;
    config.interface_type = mode == "tun" ? tunnel::interface::type::tun
                                          : tunnel::interface::type::tap;
    tunnel::interface iface1;
    iface1.create(config);
    iface1.set_log_callback(log1);
    iface1.monitor().enable_log();
    iface1.set_ipv4(tunnel_address);
    iface1.set_ipv4_netmask("255.255.255.0");
    iface1.set_mtu(1500);
    iface1.up();
    auto in_fd = iface1.native_handle();
    assert(in_fd > 0 && "Invalid file descriptor");
    auto rx = asio::posix::stream_descriptor(io);
    rx.assign(in_fd);
    rx_tun_tx_udp(rx, udp_socket, peer);
    rx_udp_tx_tun(udp_socket, rx, peer);
    io.run();

    return 0;
}
