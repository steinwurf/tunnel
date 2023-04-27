// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>

namespace tunnel
{

/// The log level
enum class log_level : uint8_t
{
    state = 0,
    debug,
    error
};

}
