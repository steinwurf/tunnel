#include <CLI/CLI.hpp>
#include <asio.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <tunnel/tap_interface.hpp>
#include <tunnel/tun_interface.hpp>

int main(int argc, char** argv)
{

    CLI::App app{"Tunnel back-to-back test app"};

    std::string mode = "";
    app.add_option("-mode", mode, "Tunnel mode tunp/tap")->required();

    auto log1 = [](auto, const std::string& message, auto)
    { std::cout << "iface1: " << message << std::endl; };

    auto log2 = [](auto, const std::string& message, auto)
    { std::cout << "iface2: " << message << std::endl; };

    CLI11_PARSE(app, argc, argv);
    asio::io_context io;

    // auto tunnel_runner = [&](int in_fd, int out_fd)
    // {
    //     assert(in_fd > 0 && "Invalid file descriptor");
    //     assert(out_fd > 0 && "Invalid file descriptor");
    //     auto rx = asio::posix::stream_descriptor(io, in_fd);
    //     auto tx = asio::posix::stream_descriptor(io, out_fd);
    //     uint8_t buffer[2000];
    //     auto rxtx = [&]()
    //     {
    //         rx.async_read_some(asio::buffer(buffer, sizeof(buffer)),
    //                            [&](std::error_code&, std::size_t) { rxtx(); }

    //         );
    //     };
    //     rxtx();
    // };

    if (mode == "tun")
    {
        tunnel::tun_interface iface1;
        tunnel::tun_interface iface2;
        iface1.set_log_callback(log1);
        iface1.monitor().enable_log();
        iface2.set_log_callback(log2);
        iface2.monitor().enable_log();

        iface1.create();
        iface1.set_ipv4("10.0.0.1");
        iface1.set_ipv4_netmask("255.255.255.0");
        iface1.set_mtu(1500);
        iface1.up();
        iface2.create();
        iface2.set_ipv4("10.0.0.2");
        iface2.set_ipv4_netmask("255.255.255.0");
        iface2.set_mtu(1500);
        iface2.up();
    }
    else if (mode == "tap")
    {
        tunnel::tap_interface iface1;
        tunnel::tap_interface iface2;
        iface1.set_log_callback(log1);
        iface1.monitor().enable_log();
        iface2.set_log_callback(log2);
        iface2.monitor().enable_log();

        iface1.create();
        iface1.set_ipv4("10.0.0.1");
        iface1.set_ipv4_netmask("255.255.255.0");
        iface1.set_mtu(1500);
        iface1.up();
        iface2.create();
        iface2.set_ipv4("10.0.0.2");
        iface2.set_ipv4_netmask("255.255.255.0");
        iface2.set_mtu(1500);
        iface2.up();
    }
    return 0;
}
