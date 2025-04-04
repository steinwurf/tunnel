// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <functional>
#include <string>

#include "log_actions.hpp"
#include "log_callback.hpp"
#include "log_level.hpp"
#include "visit_callback.hpp"

namespace tunnel
{

struct monitor
{
    /// Visit all underlying objects and print their path.
    /// The path can be used for filtering both the log and metrics.
    ///
    /// @param callback The callback that handles the visited objects.
    virtual void visit(const visit_callback& callback) const = 0;

    /// Enable logging.
    /// @param log The log callback function
    /// @param level The log level to enable
    /// @param pattern determines which monitors the filter applies to.
    virtual void enable_log(const log_callback& log,
                            log_level level = log_level::state,
                            const std::string& pattern = {}) = 0;

    /// Enable logging.
    /// @param log The log callback function
    /// @param actions The actions to enable logging for
    /// @param pattern determines which monitors the filter applies to.
    virtual void enable_log(const log_callback& log, const log_actions& actions,
                            const std::string& pattern = {}) = 0;

    /// Disables all the logging.
    virtual void disable_log() = 0;
};
}
