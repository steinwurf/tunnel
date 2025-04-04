// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>
#include <vector>

namespace tunnel
{
/// A structure to define a set of actions
struct log_actions
{
    /// Active actions constructor
    template <typename... Actions>
    log_actions(Actions... actions) :
        actions(std::vector<uint32_t>{actions.poke_hash...})
    {
    }

    /// The list of actions to enable.
    std::vector<uint32_t> actions;
};
}
