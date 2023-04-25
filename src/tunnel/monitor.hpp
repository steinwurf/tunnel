// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <functional>
#include <string>

#include "log_callback.hpp"
#include "log_level.hpp"

namespace tunnel
{

struct monitor
{
    /// @return The type.
    virtual auto type() const -> const std::string& = 0;

    /// @return The path.
    virtual auto path() const -> const std::string& = 0;

    /// Enables the log.
    ///
    /// @param level The log level.
    /// @param callback The callback that handles the log message.
    /// @param type_filter The type filter determines which log messages
    ///                    are handled by the callback.
    /// @param path_filter The path filter determines which log messages
    ///                    are handled by the callback.
    virtual void enable_log(log_level level, const log_callback& callback,
                            const std::string& type_filter = "*",
                            const std::string& path_filter = "*") = 0;

    /// Disables all the logging.
    virtual void disable_log() = 0;
};
}
