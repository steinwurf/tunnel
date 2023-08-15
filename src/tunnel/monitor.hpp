// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <functional>
#include <string>

#include "log_callback.hpp"
#include "log_level.hpp"
#include "visit_callback.hpp"

namespace tunnel
{

struct monitor
{
    /// Visit all underlying objects and print their type and path.
    /// The type and path can be used for filtering both the log and metrics.
    ///
    /// @param callback The callback that handles the visited objects.
    virtual void visit(const visit_callback& callback) const = 0;

    /// Enable the log. If type and path filter is given, the log will
    /// be propagated to all underlying objects matching the given
    /// type and path filter.
    /// @param callback The callback to receive the log messages
    /// @param level The log level
    /// @param type_filter The type filter "*" matches all types.
    /// @param path_filter The path filter "*" matches all paths.
    virtual void enable_log(const log_callback& callback,
                            log_level level = log_level::state,
                            const std::string& type_filter = "",
                            const std::string& path_filter = "") = 0;

    /// Disables all the logging.
    virtual void disable_log() = 0;
};
}
