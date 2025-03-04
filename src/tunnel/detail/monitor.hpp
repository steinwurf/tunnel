// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <any>
#include <string>

#include "action.hpp"

#include "../log_callback.hpp"
#include "../log_level.hpp"

#include <abacus/metric.hpp>
#include <poke/monitor.hpp>

#include "../monitor.hpp"

namespace tunnel
{
namespace detail
{
struct monitor : tunnel::monitor
{
    static_assert(static_cast<poke::log_level>(tunnel::log_level::state) ==
                      poke::log_level::state,
                  "tunnel::log_level::state is not the same value as "
                  "poke::log_level::state");
    static_assert(static_cast<poke::log_level>(tunnel::log_level::debug) ==
                      poke::log_level::debug,
                  "tunnel::log_level::debug is not the same value as "
                  "poke::log_level::debug");
    static_assert(static_cast<poke::log_level>(tunnel::log_level::info) ==
                      poke::log_level::info,
                  "tunnel::log_level::info is not the same value as "
                  "poke::log_level::info");
    static_assert(static_cast<poke::log_level>(tunnel::log_level::warn) ==
                      poke::log_level::warn,
                  "tunnel::log_level::warn is not the same value as "
                  "poke::log_level::warn");
    static_assert(static_cast<poke::log_level>(tunnel::log_level::error) ==
                      poke::log_level::error,
                  "tunnel::log_level::error is not the same value as "
                  "poke::log_level::error");
    static_assert(static_cast<poke::log_level>(tunnel::log_level::fatal) ==
                      poke::log_level::fatal,
                  "tunnel::log_level::fatal is not the same value as "
                  "poke::log_level::fatal");
    monitor(const std::string& name,
            const std::map<abacus::name, abacus::info>& metrics_infos,
            const std::vector<poke::action>& actions) :
        m_monitor(name, metrics_infos, actions)
    {
    }

    virtual void visit(const visit_callback& callback) const override
    {
        m_monitor.visit([&callback](const poke::monitor_view view)
                        { callback(view.path()); });
    }

    void enable_log(const log_callback& log, log_level level = log_level::state,
                    const std::string& pattern = "") override
    {
        m_monitor.enable_log(
            [log](poke::log_level level, const std::string_view& message)
            { log(static_cast<tunnel::log_level>(level), message); },
            static_cast<poke::log_level>(level), pattern);
    }

    void enable_log(const log_callback& log, const log_actions& actions,
                    const std::string& pattern = "") override
    {
        m_monitor.enable_log(
            [log](poke::log_level level, const std::string_view& message)
            { log(static_cast<tunnel::log_level>(level), message); },
            {actions.actions}, pattern);
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
