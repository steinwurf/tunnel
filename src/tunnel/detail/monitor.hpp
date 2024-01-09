// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <any>
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

    virtual void set_log_callback(const log_callback& callback) override
    {
        m_log_callback = callback;
        m_monitor.set_log_callback(
            [this](poke::log_level level, const std::string& message,
                   const std::any& user_data) {
                m_log_callback(static_cast<tunnel::log_level>(level), message,
                               user_data);
            });
    }

    virtual void enable_log(log_level level = log_level::state,
                            const std::string& type_filter = "",
                            const std::string& path_filter = "",
                            std::any user_data = {}) override
    {
        m_monitor.enable_log(static_cast<poke::log_level>(level), type_filter,
                             path_filter, user_data);
    }

    virtual void disable_log() override
    {
        m_monitor.disable_log();
    }

    auto is_log_enabled(log_level level) const -> bool
    {
        return m_monitor.is_log_enabled(static_cast<poke::log_level>(level));
    }

public:
    poke::monitor m_monitor;

    tunnel::log_callback m_log_callback;
};
}
}
