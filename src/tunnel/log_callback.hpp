// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <string>

#include "delegate.hpp"

namespace tunnel
{

/// The logging callback
using log_callback = delegate<void(const std::string&)>;

}
