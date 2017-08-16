// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/tun_interface.hpp>

#include <iostream>
#include <thread>
#include <vector>

int main()
{
    tunnel::tun_interface t;

    std::string name("testtun");

    auto fd = t.create_tun_interface(name);
    if (fd < 0)
    {
        perror("Allocating interface");
        close(fd);
        exit(1);
    }

    (void) fd;

    std::cout << "Created tun interface " << name << std::endl;

    std::vector<char> buffer(2000);

    while (true)
    {
        std::cout << "Reading from interface" << std::endl;
        // Note that "buffer" should be at least the MTU size of the interface,
        // eg 1500 bytes
        int nread = read(fd, buffer.data(), buffer.size());

        if (nread < 0)
        {
            perror("Reading from interface");
            close(fd);
            exit(1);
        }

        /* Do whatever with the data */
        printf("Read %d bytes from device %s\n", nread, name.c_str());
    }

    return 0;
}
