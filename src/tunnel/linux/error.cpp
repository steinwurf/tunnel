// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include "error.hpp"

#include <cassert>
#include <string>

namespace tunnel
{
namespace linux
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

const linux_category& error_category()
{
    static linux_category category;
    return category;
}

std::error_code make_error_code(linux_error error)
{
    return {static_cast<int>(error), error_category()};
}

}
}
