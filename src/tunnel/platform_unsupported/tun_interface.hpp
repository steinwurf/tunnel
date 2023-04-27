
#include "layer_tun.hpp"

#include "../detail/layer_final.hpp"
#include "../detail/layer_monitor.hpp"
namespace tunnel
{
namespace platform_unsupported
{
// clang-format off
struct tun_interface : public
    layer_tun<
    detail::layer_monitor<
    detail::layer_final<tun_interface>>>
// clang-format on
{
    static bool is_platform_supported()
    {
        return false;
    }

    static auto type() -> std::string
    {
        return "tunnel::platform_unsupported::tun_interface";
    }
};

}
}
