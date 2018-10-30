// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

#include <cstdint>
#include <string>

namespace tunnel
{
namespace linux
{
namespace detail
{
std::string if_indextoname(uint32_t index);
}
}
}
