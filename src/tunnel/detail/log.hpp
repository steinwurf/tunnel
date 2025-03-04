// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <poke/log.hpp>

namespace tunnel
{
namespace detail
{
struct log
{
    using boolean = poke::log::boolean;
    using integer = poke::log::integer;
    using uinteger = poke::log::uinteger;
    using float64 = poke::log::float64;
    using str = poke::log::str;
    using ptr = poke::log::ptr;
};
}
}
