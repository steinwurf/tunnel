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
tun_interface::tun_interface()
{
}

// @param devname: the name of an interface. May be an empty string. In this
// case, the kernel chooses the interface name and sets devname to this.
// @return the tun file descriptor
int tun_interface::create_tun_interface(std::string& devname)
{
    int kernel_socket = -1;
    kernel_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct ifreq ifr;
    int fd, err;
    const char* clonedev = "/dev/net/tun";

    // open the clone device
    if ((fd = open(clonedev, O_RDWR)) < 0)
    {
        return fd;
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
    if ((err = ioctl(fd, TUNSETIFF, (void*) &ifr)) < 0)
    {
        close(fd);
        return err;
    }

    // if the operation was successful, write back the name of the
    // interface to the variable "dev", so the caller can know
    // it. Note that the caller MUST reserve space in *dev (see calling
    // code below)
    devname = std::string(ifr.ifr_name);

    // Set the device up
    std::cout << "Getting flags from " << devname << std::endl;

    // Get flags from interface
    if ((err = ioctl(kernel_socket, SIOCGIFFLAGS, &ifr)) == -1)
    {
        close(fd);
        return err;
    }

    ifr.ifr_flags |= IFF_UP;

    if ((err = ioctl(kernel_socket, SIOCSIFFLAGS, &ifr)) < 0)
    {
        close(fd);
        return err;
    }

    return fd;
}
}
