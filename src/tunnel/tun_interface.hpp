// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

// Linux TUN/TAP includes
#include <net/if.h>
#include <net/route.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>

// POSIX
#include <fcntl.h>      // O_RDWR
#include <unistd.h>     // open(), close()

#include <string>
#include <memory>
#include <functional>
#include <system_error>

#include <boost/asio.hpp>

namespace tunnel
{
class tun_interface
{
public:

    using io_handler = std::function<void(const std::error_code&,uint32_t)>;

    // constructor
    tun_interface(boost::asio::io_service& io,
                  int tun_fd,
                  const std::string& devname);

    ~tun_interface();

    static std::unique_ptr<tun_interface> make_tun_interface(
        boost::asio::io_service& io,
        std::string& devname,
        std::error_code& error);

    void set_ipv4(const std::string& address, std::error_code& error);
    void set_mtu(uint32_t mtu, std::error_code& error);

    bool is_up(std::error_code& error) const;
    void up(std::error_code& error);

    bool is_down(std::error_code& error) const;
    void down(std::error_code& error);

    std::string name() const;

    // Read buffers / packets sent to this interface from the kernel.
    // That is, "outbound traffic".
    void async_read(std::vector<uint8_t>& buffer, io_handler callback);

    // Write buffers / packets to the kernel from this interface.
    // That is, "inbound traffic".
    void async_write(const std::vector<uint8_t>& buffer,
                     io_handler callback);

    // Write buffers / packets to the kernel from this interface.
    // That is, "inbound traffic".
    void write(const std::vector<uint8_t>& buffer, std::error_code& error);

    // Set default route to this interface
    void set_default_route(std::error_code& error);

    // Remove default route from this interface
    void remove_default_route(std::error_code& error);

private:

    // The dev name
    std::string m_name;

    // Internal kernel socket used for ioctl calls
    int m_kernel_socket;

    // The tun dev file descriptor
    int m_tun_fd = -1;

    // The tun fd stream descriptor
    boost::asio::posix::stream_descriptor m_tun_stream;
};
}
