// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once
#include <system_error>

namespace tunnel
{
namespace platform_linux
{

enum class linux_error
{

#define ERROR_TAG(id, msg) id,
#include "error_tags.hpp"
#undef ERROR_TAG
    undefined

};

auto make_error_code(linux_error error) -> std::error_code;

}
}
