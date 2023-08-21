// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <tunnel/tun_interface.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
extern "C"
{

#include <sys/types.h>

#include <linux/if_tun.h>
    // #include <linux/virtio_net.h>
}
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
struct virtio_net_hdr
{
    /* See VIRTIO_NET_HDR_F_* */
    __u8 flags;
    /* See VIRTIO_NET_HDR_GSO_* */
    __u8 gso_type;
    __u16 hdr_len;     /* Ethernet + IP + tcp/udp hdrs */
    __u16 gso_size;    /* Bytes to append to hdr_len per frame */
    __u16 csum_start;  /* Position to start checksumming from */
    __u16 csum_offset; /* Offset after that to place checksum */
};

/// In this example we read data from the TUN interface
///
/// First run this program:
///
///   ./build/linux/examples/backtobacktun
///
/// Then in a different terminal run e.g.
///
///   ping 10.0.0.2

int main()
{
    auto log1 = [](auto, const std::string& message)
    { std::cout << "iface1: " << message << std::endl; };

    auto log2 = [](auto, const std::string& message)
    { std::cout << "iface2: " << message << std::endl; };

    tunnel::tun_interface iface1;
    tunnel::tun_interface iface2;
    iface1.set_log_callback(log1);
    iface1.monitor().enable_log();
    iface2.set_log_callback(log2);
    iface2.monitor().enable_log();

    iface1.create();
    iface1.set_ipv4("10.0.0.1");
    iface1.set_ipv4_netmask("255.255.255.0");
    iface1.set_mtu(6000);
    iface1.up();

    // Enable offload
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    // get the current flags

    // Enable offloa
    int off_load_flags = TUN_F_CSUM | TUN_F_TSO4 | TUN_F_UFO;

    int offload =
        ::ioctl(iface1.native_handle(), TUNSETOFFLOAD, off_load_flags);
    if (offload < 0)
    {
        std::cout << "Error setting offload" << std::endl;
        exit(1);
    }

    // iface2.create();
    // iface2.set_ipv4("10.0.0.2");
    // iface2.set_ipv4_netmask("255.255.255.0");
    // iface2.set_mtu(1500);
    // iface2.up();

    std::vector<uint8_t> buffer(1000000);

    uint32_t packets = 0;

    while (true)
    {

        // msg read from tun
        int recv = read(iface1.native_handle(), buffer.data(), buffer.size());

        struct virtio_net_hdr* hdr = (struct virtio_net_hdr*)buffer.data();

        std::cout << "flags: " << (int)hdr->flags << std::endl;
        std::cout << "gso_type: " << (int)hdr->gso_type << std::endl;

        if (recv < 0)
        {
            std::cout << "Error reading from TUN" << std::endl;
            perror("read");
            exit(1);
        }

        std::cout << packets << ": Read " << recv << " bytes" << std::endl;
        ++packets;
    }

    return 0;
}
