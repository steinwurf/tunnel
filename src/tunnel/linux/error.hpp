#pragma once
#include <system_error>

namespace tunnel
{
namespace linux
{

enum class linux_error
{

#define ERROR_TAG(id, msg) id,
#include "error_tags.hpp"
#undef ERROR_TAG
    undefined

};

std::error_code make_error_code(linux_error error);

}
}