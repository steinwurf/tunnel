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
std::error_code to_std_error_code(boost::system::error_code error)
{
    return std::error_code(error.value(), std::generic_category());
}

void io_handler_proxy(const tun_interface::io_handler& callback,
                      const boost::system::error_code& error,
                      uint32_t bytes_transferred)
{
    callback(to_std_error_code(error), bytes_transferred);
}

// Helper function for reading flags from interface
struct ifreq read_interface_flags(int socket_fd,
                                  const std::string& name,
                                  std::error_code& error)
{
    struct ifreq ifr;

    std::strncpy(ifr.ifr_name, name.c_str(), name.size());

    if (ioctl(socket_fd, SIOCGIFFLAGS, &ifr) == -1)
    {
        error = make_error_code(errno);
        return ifr;
    }

    return ifr;
}


// @param io the io service to be used for async operation
// @param devname: the name of an interface. May be an empty string. In this
// case, the kernel chooses the interface name and sets devname to this.
// @return the tun file descriptor
std::unique_ptr<tun_interface> tun_interface::make_tun_interface(
    boost::asio::io_service& io,
    std::string& devname,
    std::error_code& error)
{
    struct ifreq ifr;
    const char* clonedev = "/dev/net/tun";
    int fd;
    // open the clone device
    if ((fd = open(clonedev, O_RDWR)) < 0)
    {
        error = make_error_code(errno);
        return nullptr;
    }

    // preparation of the struct ifr, of type "struct ifreq"
    std::memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TUN;

    if (!devname.empty())
    {
        // if a device name was specified, put it in the structure;
        // otherwise, the kernel will try to allocate the "next" device of the
        // specified type
        std::strncpy(ifr.ifr_name, devname.c_str(), devname.size());
    }

    // try to create the device
    if (ioctl(fd, TUNSETIFF, (void*) &ifr) < 0)
    {
        close(fd);
        error = make_error_code(errno);
        return nullptr;
    }

    // if the operation was successful, write back the name of the
    // interface to the variable "dev", so the caller can know
    // it. Note that the caller MUST reserve space in *dev (see calling
    // code below)
    devname = std::string(ifr.ifr_name);

    return std::make_unique<tun_interface>(io, fd, devname);
}

tun_interface::tun_interface(boost::asio::io_service& io,
                             int tun_fd,
                             const std::string& devname)
    : m_name(devname),
      m_kernel_socket(socket(AF_INET, SOCK_STREAM, 0)),
      m_tun_fd(tun_fd),
      m_tun_stream(io)
{
    m_tun_stream.assign(m_tun_fd);
}

void tun_interface::up(std::error_code& error)
{
    struct ifreq ifr = read_interface_flags(m_kernel_socket, m_name, error);

    if (error)
    {
        return;
    }

    ifr.ifr_flags |= IFF_UP;

    if (ioctl(m_kernel_socket, SIOCSIFFLAGS, &ifr) == -1)
    {
        error = make_error_code(errno);
        return;
    }
}

void tun_interface::down(std::error_code& error)
{
    struct ifreq ifr = read_interface_flags(m_kernel_socket, m_name, error);

    ifr.ifr_flags &= ~IFF_UP;

    if (ioctl(m_kernel_socket, SIOCSIFFLAGS, &ifr) == -1)
    {
        error = make_error_code(errno);
        return;
    }
}

std::string tun_interface::name() const
{
    return m_name;
}

// @param buffer the buffer into which the data will be read.
// Ownership of the buffer is retained by the caller, which must guarantee it
// remain valid until the callback is called
// @param callback The callback to be called when the read operation
// completes. Copies will be made of the handler as required.
void tun_interface::async_read(std::vector<uint8_t>& buffer,
                               io_handler callback)
{
    m_tun_stream.async_read_some(boost::asio::buffer(buffer),
                                 std::bind(io_handler_proxy,
                                           callback,
                                           std::placeholders::_1,
                                           std::placeholders::_2));

    
}

void tun_interface::async_write(const std::vector<uint8_t>& buffer,
                                io_handler callback)
{
    boost::asio::async_write(m_tun_stream,
                             boost::asio::buffer(buffer),
                             std::bind(io_handler_proxy,
                                       callback,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}


}
