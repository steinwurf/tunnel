// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <string>

#include "delegate.hpp"

namespace tunnel
{
/// The callback is used when calling monitor::visit.
using visit_callback = delegate<void(const std::string&, const std::string&)>;
}
