// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

// Linux TUN/TAP includes
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>

// POSIX
#include <fcntl.h>      // O_RDWR
#include <unistd.h>     // open(), close()

#include <string>


namespace tunnel
{
    class tun_interface
    {
    public:

        // constructor
        tun_interface();

        int create_tun_interface(std::string& devname);
    };
}
