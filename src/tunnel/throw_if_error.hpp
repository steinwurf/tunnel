// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <system_error>

namespace tunnel
{
/// Small helper function which throws an exception if the error code
/// passed indicates an error.
inline void throw_if_error(const std::error_code& error)
{
    if (error)
    {
        throw std::system_error(error);
    }
}

}