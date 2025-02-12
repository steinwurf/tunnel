// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <initializer_list>
#include <string_view>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <poke/action.hpp>
#include <poke/hash_function.hpp>

namespace tunnel
{
namespace detail
{
struct action
{
/// Create the action tags
#define ACTION_TYPE_TAG(id, desc)                                             \
    struct _##id                                                              \
    {                                                                         \
        static constexpr std::string_view description = desc;                 \
        static constexpr std::string_view name = #id;                         \
        static constexpr auto poke_hash = poke::hash_function("tunnel_" #id); \
    };                                                                        \
    constexpr static inline _##id id{};

#include "action_tags.hpp"

#undef ACTION_TYPE_TAG

    /// Create the action registry
    constexpr static inline std::initializer_list<poke::action> poke_registry =
        {

#define ACTION_TYPE_TAG(id, desc) \
    {_##id::poke_hash, _##id::name, _##id::description},

#include "action_tags.hpp"

#undef ACTION_TYPE_TAG
    };
};

/// Make the conversion functions from the actions to json
#define ACTION_TYPE_TAG(id, desc)                                           \
    inline void to_json_property(fmt::memory_buffer& buffer, action::_##id) \
    {                                                                       \
        buffer.append(std::string_view("\"action\": \"" #id "\""));         \
    }

#include "action_tags.hpp"

#undef ACTION_TYPE_TAG
}
}
