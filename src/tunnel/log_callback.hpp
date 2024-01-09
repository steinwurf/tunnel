// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <any>
#include <string>

#include "delegate.hpp"
#include "log_level.hpp"

namespace tunnel
{

/// The logging callback
using log_callback =
    delegate<void(tunnel::log_level, const std::string&, const std::any&)>;

}
