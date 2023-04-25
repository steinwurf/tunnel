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

#include "../log_callback.hpp"
#include "../log_kind.hpp"
#include "../to_string.hpp"

namespace tunnel
{

inline void format_to(fmt::memory_buffer& buffer, tunnel::log_kind kind)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": "{}")",
                   tunnel::to_string(kind));
}

namespace detail
{

inline void format_to(fmt::memory_buffer& buffer, log::boolean element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value ? "true" : "false");
}

inline void format_to(fmt::memory_buffer& buffer, log::integer element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value);
}

inline void format_to(fmt::memory_buffer& buffer, log::uinteger element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value);
}

inline void format_to(fmt::memory_buffer& buffer, log::float64 element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": {})", element.name,
                   element.value);
}

inline void format_to(fmt::memory_buffer& buffer, log::str element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": "{}")", element.name,
                   element.value);
}

inline void format_to(fmt::memory_buffer& buffer, log::ptr element)
{
    fmt::format_to(std::back_inserter(buffer), R"("{}": "{}")", element.name,
                   (uint64_t)element.value);
}

}
}
