// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <string>

#include "log_kind.hpp"

#include "../log_callback.hpp"
#include "../log_level.hpp"

#include <abacus/metric_info.hpp>
#include <poke/monitor.hpp>

#include "../monitor.hpp"

namespace tunnel
{
namespace detail
{
struct monitor : tunnel::monitor
{
    static auto to_poke_log_level(log_level level) -> poke::log_level
    {
        switch (level)
        {
        case log_level::state:
            return poke::log_level::state;
        case log_level::debug:
            return poke::log_level::debug;
        case log_level::info:
            return poke::log_level::info;
        case log_level::warn:
            return poke::log_level::warn;
        case log_level::error:
            return poke::log_level::error;
        case log_level::fatal:
            return poke::log_level::fatal;
        default:
            assert(false);
        }
        return poke::log_level::state;
    }

    monitor(const std::string& type,
            const std::vector<abacus::metric_info>& metrics) :
        m_monitor(type, metrics)
    {
    }

    virtual void visit(const visit_callback& callback) const override
    {
        m_monitor.visit([&callback](const poke::monitor& monitor)
                        { callback(monitor.type(), monitor.path()); });
    }

    virtual void enable_log(const log_callback& callback,
                            log_level level = log_level::state,
                            const std::string& type_filter = "",
                            const std::string& path_filter = "") override
    {
        m_monitor.enable_log({callback.invoke(), callback.storage()},
                             to_poke_log_level(level), type_filter,
                             path_filter);
    }

    virtual void disable_log() override
    {
        m_monitor.disable_log();
    }

public:
    poke::monitor m_monitor;
};
}
}
