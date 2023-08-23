// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <cstdint>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include "log.hpp"
#include "log_kind.hpp"
#include "to_string.hpp"

#include "../log_callback.hpp"

namespace tunnel
{
namespace detail
{

inline void to_json_property(fmt::memory_buffer& buffer, log_kind kind)
{
    fmt::format_to(std::back_inserter(buffer), R"("kind": "{}")",
                   to_string(kind));
}
inline void to_json_property(fmt::memory_buffer& buffer, log::boolean element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value ? "true" : "false");
}

inline void to_json_property(fmt::memory_buffer& buffer, log::integer element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value);
}

inline void to_json_property(fmt::memory_buffer& buffer, log::uinteger element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value);
}

inline void to_json_property(fmt::memory_buffer& buffer, log::float64 element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value);
}

inline void to_json_property(fmt::memory_buffer& buffer, log::str element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": "{}")", element.name,
                   element.value);
}

inline void to_json_property(fmt::memory_buffer& buffer, log::ptr element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": "{}")", element.name,
                   (uint64_t)element.value);
}

}
}
