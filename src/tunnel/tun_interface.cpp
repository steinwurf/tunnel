// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include "tun_interface.hpp"

#include <cstring>
#include <iostream>

namespace tunnel
{

// Helper function for creating error codes
std::error_code make_error_code(int value)
{
    return std::error_code(value, std::generic_category());
}

// Helper function for converting boost errors to std errors
std::error_code to_std_error_code(const boost::system::error_code& error)
{
    return std::error_code(error.value(), std::generic_category());
}

// Proxy function for converting boost error codes to std error codes
void io_handler_proxy(const tun_interface::io_handler& callback,
                      const boost::system::error_code& error,
                      uint32_t bytes_transferred)
{
    callback(to_std_error_code(error), bytes_transferred);
}

// Helper function for reading flags from interface
struct ifreq read_interface_info(int socket_file_descriptor,
                                 const std::string& device_name,
                                 std::error_code& error)
{
    assert(!error);

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));

    device_name.copy(ifr.ifr_name, device_name.size());

    if (ioctl(socket_file_descriptor, SIOCGIFFLAGS, &ifr) == -1)
    {
        error = make_error_code(errno);
        return ifr;
    }

    return ifr;
}

// @param io the io service to be used for async operation
// @param wanted_devname: the wamted name of an interface. If empty
// the kernel chooses the interface name.
// @return the tun file descriptor
std::unique_ptr<tun_interface> tun_interface::make_tun_interface(
    boost::asio::io_service& io,
    const std::string& wanted_device_name,
    std::error_code& error)
{
    assert(!error);

    struct ifreq ifr;
    const char* tun_device = "/dev/net/tun";
    int file_descriptor;

    if ((file_descriptor = open(tun_device, O_RDWR)) < 0)
    {
        error = make_error_code(errno);
        return nullptr;
    }

    std::memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TUN;

    if (!wanted_device_name.empty())
    {
        // if a device name was specified, put it in the structure;
        // otherwise, the kernel will try to allocate the "next" device of the
        // specified type
        wanted_device_name.copy(ifr.ifr_name, wanted_device_name.size());
    }

    // try to create the device
    if (ioctl(file_descriptor, TUNSETIFF, (void*) &ifr) < 0)
    {
        close(file_descriptor);
        error = make_error_code(errno);
        return nullptr;
    }

    auto device_name = std::string(ifr.ifr_name);
    return std::make_unique<tun_interface>(io, file_descriptor, device_name);
}

tun_interface::tun_interface(boost::asio::io_service& io,
                             int file_descriptor,
                             const std::string& device_name) :
    m_device_name(device_name),
    m_socket(socket(AF_INET, SOCK_STREAM, 0)),
    m_file_descriptor(file_descriptor),
    m_stream_descriptor(io)
{
    m_stream_descriptor.assign(m_file_descriptor);
}

tun_interface::~tun_interface()
{
    m_stream_descriptor.cancel();
    m_stream_descriptor.close();
    m_stream_descriptor.release();
    close(m_socket);
    close(m_file_descriptor);
}

bool tun_interface::is_up(std::error_code& error) const
{
    assert(!error);

    struct ifreq ifr = read_interface_info(m_socket, m_device_name, error);

    return (ifr.ifr_flags & IFF_UP) != 0;
}

void tun_interface::up(std::error_code& error)
{
    assert(!error);

    struct ifreq ifr = read_interface_info(m_socket, m_device_name, error);

    if (error)
    {
        return;
    }

    ifr.ifr_flags |= IFF_UP;

    if (ioctl(m_socket, SIOCSIFFLAGS, &ifr) == -1)
    {
        error = make_error_code(errno);
        return;
    }
}

bool tun_interface::is_down(std::error_code& error) const
{
    assert(!error);

    return !is_up(error);
}

void tun_interface::down(std::error_code& error)
{
    assert(!error);

    struct ifreq ifr = read_interface_info(m_socket, m_device_name, error);

    ifr.ifr_flags &= ~IFF_UP;

    if (ioctl(m_socket, SIOCSIFFLAGS, &ifr) == -1)
    {
        error = make_error_code(errno);
        return;
    }
}

void tun_interface::set_ipv4(const std::string& address, std::error_code& error)
{
    assert(!error);

    boost::system::error_code ec;
    auto addr = boost::asio::ip::address_v4::from_string(address, ec);
    if (ec)
    {
        error = to_std_error_code(ec);
        return;
    }
    auto mask = boost::asio::ip::address_v4::from_string("255.255.255.0");
    auto bcast = boost::asio::ip::address_v4::broadcast(addr, mask);

    struct ifreq ifr = read_interface_info(m_socket, m_device_name, error);
    if (error)
    {
        return;
    }

    struct sockaddr_in* addr_in = (struct sockaddr_in*) &ifr.ifr_addr;

    addr_in->sin_family = AF_INET;

    std::vector<std::pair<int, boost::asio::ip::address_v4>> addr_config =
        {
            {SIOCSIFADDR, addr}, {SIOCSIFNETMASK, mask}, {SIOCSIFBRDADDR, bcast}
        };

    for (const auto& pair : addr_config)
    {
        int res = inet_pton(
            AF_INET, pair.second.to_string().c_str(), &addr_in->sin_addr);
        if (res == 0)
        {
            error = std::make_error_code(std::errc::invalid_argument);
            return;
        }
        else if (res < 0)
        {
            error = make_error_code(errno);
            return;
        }

        if (ioctl(m_socket, pair.first, &ifr) != 0)
        {
            error = make_error_code(errno);
            return;
        }

        std::cout << "Setting config " << pair.second << std::endl;
    }
}

uint32_t tun_interface::mtu(std::error_code& error) const
{
    assert(!error);

    struct ifreq ifr = read_interface_info(m_socket, m_device_name, error);
    if (error)
    {
        return 0;
    }

    return ifr.ifr_mtu;
}

void tun_interface::set_mtu(uint32_t mtu, std::error_code& error)
{
    assert(!error);

    if (mtu < ETH_HLEN || mtu > 65535)
    {
        error = std::make_error_code(std::errc::invalid_argument);
        return;
    }

    struct ifreq ifr = read_interface_info(m_socket, m_device_name, error);
    if (error)
    {
        return;
    }

    ifr.ifr_mtu = mtu;

    if (ioctl(m_socket, SIOCSIFMTU, &ifr) != 0)
    {
        error = make_error_code(errno);
        return;
    }
}

std::string tun_interface::device_name() const
{
    return m_device_name;
}

// @param buffer the buffer into which the data will be read.
// Ownership of the buffer is retained by the caller, which must guarantee it
// remain valid until the callback is called
// @param callback The callback to be called when the read operation
// completes. Copies will be made of the handler as required.
void tun_interface::async_read(
    uint8_t* data, uint32_t size, io_handler callback)
{
    m_stream_descriptor.async_read_some(boost::asio::buffer(data, size),
                                        std::bind(io_handler_proxy,
                                                  callback,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2));
}

void tun_interface::async_write(
    const uint8_t* data, uint32_t size, io_handler callback)
{
    boost::asio::async_write(m_stream_descriptor,
                             boost::asio::buffer(data, size),
                             std::bind(io_handler_proxy,
                                       callback,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}

void tun_interface::write(
    const uint8_t* data, uint32_t size, std::error_code& error)
{
    assert(!error);

    boost::system::error_code ec;
    boost::asio::write(
        m_stream_descriptor, boost::asio::buffer(data, size), ec);
    error = to_std_error_code(ec);
}

void tun_interface::set_default_route(std::error_code& error)
{
    assert(!error);

    struct rtentry route;
    std::memset(&route, 0, sizeof(route));

    char ifname[IFNAMSIZ] = { 0 };
    m_device_name.copy(ifname, m_device_name.size());

    route.rt_dev = ifname;
    route.rt_flags = RTF_UP; // | RTF_GATEWAY;

    struct sockaddr_in* gateway = (struct sockaddr_in*) &route.rt_gateway;
    gateway->sin_family = AF_INET;
    gateway->sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in* dest = (struct sockaddr_in*) &route.rt_dst;
    dest->sin_family = AF_INET;
    dest->sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in* mask = (struct sockaddr_in*) &route.rt_genmask;
    mask->sin_family = AF_INET;
    mask->sin_addr.s_addr = INADDR_ANY;

    if (ioctl(m_socket, SIOCADDRT, &route) != 0)
    {
        error = make_error_code(errno);
    }
}

void tun_interface::remove_default_route(std::error_code& error)
{
    assert(!error);

    struct rtentry route;
    std::memset(&route, 0, sizeof(route));

    char ifname[IFNAMSIZ] = { 0 };
    m_device_name.copy(ifname, m_device_name.size());

    route.rt_dev = ifname;
    route.rt_flags = RTF_UP | RTF_GATEWAY;

    struct sockaddr_in* gateway = (struct sockaddr_in*) &route.rt_gateway;
    gateway->sin_family = AF_INET;
    gateway->sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in* dest = (struct sockaddr_in*) &route.rt_dst;
    dest->sin_family = AF_INET;
    dest->sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in* mask = (struct sockaddr_in*) &route.rt_genmask;
    mask->sin_family = AF_INET;
    mask->sin_addr.s_addr = INADDR_ANY;

    if (ioctl(m_socket, SIOCDELRT, &route) != 0)
    {
        error = make_error_code(errno);
    }
}
}
