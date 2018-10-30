// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

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