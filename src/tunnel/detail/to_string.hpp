// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <string>

#include "log_kind.hpp"

namespace tunnel
{
namespace detail
{
/// Convert a log_kind to a string
/// @param kind The log_kind
/// @return The human-readable string value
auto to_string(log_kind kind) -> std::string;

}
}
