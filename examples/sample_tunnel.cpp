#include <boost/asio.hpp>
#include <tunnel/tun_interface.hpp>

#include <cassert>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <deque>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>


/**
 * Create Tunnel:
 * Local machine (192.168.1.2):
 * sudo build/linux/examples/udp_tunnel2 --tunnel_ip 10.0.0.1 --remote_ip 192.168.1.3 --local_ip 192.168.1.2
 *
 * Remote machine (192.168.1.3):
 * sudo build/linux/examples/udp_tunnel2 --tunnel_ip 10.0.0.2 --remote_ip 192.168.1.2 --local_ip 192.168.1.3
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

    sample_tunnel(
        boost::asio::io_service& io,
        int tun_fd,
        uint32_t mtu,
        boost::asio::ip::udp::endpoint local_endpoint,
        boost::asio::ip::udp::endpoint remote_endpoint) :
        m_socket(io),
        m_mtu(mtu),
        m_stream_descriptor(io),
        m_local_endpoint(local_endpoint),
        m_remote_endpoint(remote_endpoint)
    {
        m_stream_descriptor.assign(tun_fd);
    }

    void start()
    {
        m_socket.open(m_local_endpoint.protocol());

        // bind to specified address
        m_socket.bind(m_local_endpoint);

        // allow socket to transmit broadcast packets
        m_socket.set_option(boost::asio::socket_base::broadcast(true));

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
            boost::asio::buffer(m_rx_buffer),
            std::bind(
                &sample_tunnel::handle_async_network_receive,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void handle_async_network_receive(
        const boost::system::error_code& ec,
        std::size_t bytes)
    {
        if (ec == boost::system::errc::operation_canceled)
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
        boost::asio::write(
            m_stream_descriptor,
            boost::asio::buffer(m_rx_buffer));
        async_network_receive();
    }


    void async_tun_read()
    {
        m_tx_buffer.resize(m_mtu);
        m_stream_descriptor.async_read_some(
            boost::asio::buffer(m_tx_buffer),
            std::bind(
                &sample_tunnel::handle_async_tun_read,
                this,
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void handle_async_tun_read(
        const boost::system::error_code& ec,
        std::size_t bytes)
    {
        if (ec == boost::system::errc::operation_canceled)
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
        m_socket.send_to(boost::asio::buffer(m_tx_buffer), m_remote_endpoint);
        async_tun_read();
    }

private:

    boost::asio::ip::udp::socket m_socket;

    const uint32_t m_mtu;

    // The tun file descriptor stream descriptor
    boost::asio::posix::stream_descriptor m_stream_descriptor;

    const boost::asio::ip::udp::endpoint m_local_endpoint;
    const boost::asio::ip::udp::endpoint m_remote_endpoint;

    std::vector<uint8_t> m_rx_buffer;
    std::vector<uint8_t> m_tx_buffer;
};

int main(int argc, char* argv[])
{
    namespace bpo = boost::program_options;

    std::string local_ip;
    std::string remote_ip;
    uint16_t port;
    std::string tunnel_ip;

    // Parse the prorgram options
    bpo::options_description options("Commandline Options");

    options.add_options()
    ("tunnel_ip,t", bpo::value<std::string>(&tunnel_ip)->required(),
     "Specify the IPv4 address to set on the created tunnel "
     "interface [required]")
    ("local_ip,l", bpo::value<std::string>(&local_ip)->required(),
     "Specify the IPv4 address of the local interface that the tunnel "
     "should send to and receive from [required]")
    ("remote_ip,r", bpo::value<std::string>(&remote_ip)->required(),
     "Specify the remote IPv4 address the tunnel should send to and receive "
     "from [required]")
    ("port,p", bpo::value<uint16_t>(&port)->default_value(9999),
     "Set the port to use for the udp tunnel")
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

    tunnel::tun_interface iface;
    iface.enable_log_stdout();

    iface.create();
    iface.up();

    iface.set_ipv4(tunnel_ip);
    iface.set_ipv4_netmask("255.255.255.0");

    boost::asio::io_service io;

    auto local_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::from_string(local_ip),
        port);

    auto remote_endpoint = boost::asio::ip::udp::endpoint(
        boost::asio::ip::address_v4::from_string(remote_ip),
        port);

    sample_tunnel tunnel(
        io,
        iface.native_handle(),
        iface.mtu(),
        local_endpoint,
        remote_endpoint
    );

    tunnel.start();

    io.run();

    return 0;
}
