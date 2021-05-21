// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <sys/ioctl.h>
#include <system_error>

#include "scoped_file_descriptor.hpp"

namespace tunnel
{
namespace linux
{
namespace detail
{
template <class T>
inline std::string to_string(const T& s)
{
    std::stringstream ss;
    ss << s;
    return ss.str();
}
}

/// Passes an ioctl request to the TUN driver that we are using or
/// sets the error_code.
class layer_log
{
public:
    template <class... Args>
    void write_log(const Args&... args) const
    {
        if (!m_log)
        {
            return;
        }

        std::string result;
        int unpack[]{0, (result += detail::to_string(args), 0)...};
        (void)unpack;
        std::cout << result << std::endl;
    }

    void enable_log_stdout()
    {
        m_log = true;
    }

    void disable_log_stdout()
    {
        m_log = false;
    }

    bool is_log_enabled() const
    {
        return m_log;
    }

private:
    bool m_log = false;
};
}
}
