

#pragma once

namespace tunnel
{
namespace detail
{
template <class Stack>
class layer_final
{
public:
    auto stack() -> Stack&
    {
        return static_cast<Stack&>(*this);
    }

    auto stack() const -> const Stack&
    {
        return static_cast<const Stack&>(*this);
    }
};
}
}
