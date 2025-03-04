// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <functional>
#include <string>

#include "log_level.hpp"

namespace tunnel
{

/// The logging callback
using log_callback =
    std::function<void(tunnel::log_level, const std::string_view& message)>;

}
