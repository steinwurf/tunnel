// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <functional>
#include <string>

namespace tunnel
{
/// The callback is used when calling monitor::visit.
using visit_callback = std::function<void(const std::string& path)>;
}
