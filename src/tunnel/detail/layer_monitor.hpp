// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

#include "monitor.hpp"

#include "format_to.hpp"
#include "log.hpp"

#include "../log_kind.hpp"
#include "../log_level.hpp"

namespace tunnel
{
namespace detail
{

template <class Super>
class layer_monitor : public Super
{
public:
    layer_monitor() : m_impl(Super::stack().type())
    {
        m_impl.m_monitor.set_log_initializer(
            [this]() { Super::stack().log_initialize(); });
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
        do_log(log_level::state, log_kind::initialized,
               std::forward<Args>(args)...);
    }

protected:
    template <class Kind, class... Args>
    void do_log(log_level level, const Kind& kind, Args&&... args) const
    {
        m_impl.m_monitor.log(monitor::to_poke_log_level(level), kind,
                             std::forward<const Args>(args)...);
    }

private:
    tunnel::detail::monitor m_impl;
};
}
}
