// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

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
