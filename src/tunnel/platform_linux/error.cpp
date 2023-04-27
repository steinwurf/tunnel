// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <platform/config.hpp>

#if defined(PLATFORM_LINUX)

#include "error.hpp"

#include <cassert>
#include <string>

namespace tunnel
{
namespace platform_linux
{

class linux_category : public std::error_category
{
    virtual const char* name() const noexcept override final
    {
        return "linux_error";
    }

    virtual std::string message(int code) const override final
    {
        switch (static_cast<linux_error>(code))
        {
#define ERROR_TAG(id, msg) \
    case linux_error::id:  \
        return msg;
#include "error_tags.hpp"
#undef ERROR_TAG
        case linux_error::undefined:
            assert(0 && "Invalid error code received!");
            return "";
        };
        assert(0 && "Invalid error code received!");
        return "";
    }
};

auto error_category() -> const linux_category&
{
    static linux_category category;
    return category;
}

auto make_error_code(linux_error error) -> std::error_code
{
    return {static_cast<int>(error), error_category()};
}

}
}

#endif
