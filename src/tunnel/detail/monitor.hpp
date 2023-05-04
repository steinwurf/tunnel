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
        case log_level::error:
            return poke::log_level::error;
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

    virtual auto type() const -> const std::string& override
    {
        return m_monitor.type();
    }

    virtual auto path() const -> const std::string& override
    {
        return m_monitor.path();
    }

    virtual auto enable_log(log_level level, const log_callback& callback,
                            const std::string& type_filter = "*",
                            const std::string& path_filter = "*")
        -> void override
    {
        m_monitor.enable_log(to_poke_log_level(level),
                             {callback.invoke(), callback.storage()},
                             type_filter, path_filter);
    }

    virtual auto disable_log() -> void override
    {
        m_monitor.disable_log();
    }

public:
    poke::monitor m_monitor;
};
}
}
