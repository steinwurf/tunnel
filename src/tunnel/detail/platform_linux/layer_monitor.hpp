// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

#include "../../log_level.hpp"
#include "../action.hpp"
#include "../log.hpp"
#include "../monitor.hpp"
#include "../to_json_property.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_linux
{

template <class Super>
class layer_monitor : public Super
{
public:
    layer_monitor() :
        m_impl(Super::stack().name(), Super::stack().metrics_infos(),
               Super::stack().actions())
    {
        m_impl.m_monitor.set_log_initializer(
            [this]() { Super::stack().log_initialize(); });
    }

    auto metrics_infos() -> std::map<abacus::name, abacus::info>
    {
        return {};
    }

    auto actions() -> std::vector<poke::action>
    {
        return {};
    }

    auto monitor() -> tunnel::monitor&
    {
        return m_impl;
    }

    auto monitor() const -> const tunnel::monitor&
    {
        return m_impl;
    }

    template <class... Args>
    void log_initialize(Args&&... args)
    {
        do_log(log_level::state, action::initialized,
               std::forward<Args>(args)...);
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
}
