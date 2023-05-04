// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <platform/config.hpp>

#if defined(PLATFORM_LINUX)

#include <cassert>
#include <cstdint>
#include <net/if.h>
#include <string>

namespace tunnel
{
namespace detail
{
namespace platform_linux
{
// Bug in the Linux headers.
// https://bugzilla.redhat.com/show_bug.cgi?id=1300256
auto if_indextoname(uint32_t index) -> std::string
{

    char ifname[IF_NAMESIZE];

    ::if_indextoname(index, ifname);

    return ifname;
}
}
}
}
#endif
