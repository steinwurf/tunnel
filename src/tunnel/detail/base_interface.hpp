// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <any>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>

#include "monitor.hpp"

#include "log.hpp"
#include "log_kind.hpp"
#include "to_json_property.hpp"

#include "../log_level.hpp"

namespace tunnel
{
namespace detail
{

class base_interface
{
public:
    base_interface(const std::string& type,
                   const std::vector<abacus::metric_info>& metrics = {}) :
        m_impl(type, metrics)
    {
    }

    auto monitor() -> tunnel::monitor&
    {
        return m_impl;
    }

    auto monitor() const -> const tunnel::monitor&
    {
        return m_impl;
    }

    void set_log_callback(const tunnel::log_callback& callback)
    {
        m_impl.set_log_callback(callback);
    }

    void enable_log(log_level level = log_level::state,
                    std::string path_filter = "", std::string type_filter = "",
                    std::any user_data = {})
    {
        m_impl.enable_log(level, path_filter, type_filter, user_data);
    }

protected:
    template <class Kind, class... Args>
    void do_log(log_level level, const Kind& kind, Args&&... args) const
    {
        m_impl.m_monitor.log(static_cast<poke::log_level>(level), kind,
                             std::forward<const Args>(args)...);
    }

private:
    tunnel::detail::monitor m_impl;
};
}
}
