// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <string>

namespace tunnel
{
namespace detail
{
struct log
{
    struct boolean
    {
        const char* name;
        bool value;
    };

    struct integer
    {
        const char* name;
        int64_t value;
    };

    struct uinteger
    {
        const char* name;
        uint64_t value;
    };

    struct float64
    {
        static_assert(sizeof(double) == 8, "double not 8 bytes");

        const char* name;
        double value;
    };

    struct str
    {
        const char* name;
        const char* value;
    };

    struct ptr
    {
        const char* name;
        const uint8_t* value;
    };
};
}
}
