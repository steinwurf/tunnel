
#include "../log_kind.hpp"
#include "../log_level.hpp"

namespace tunnel
{
namespace platform_unsupported
{
template <class Super>
class layer_tun : public Super
{
    void create(const std::string& interface_name, std::error_code& error)
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "create: Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }
    void rename(const std::string& interface_name, std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_owner(const std::string& owner, std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string& group, std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string interface_name(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    bool is_up(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    bool is_down(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void up(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void down(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    int mtu(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return 0;
    }

    void set_non_persistent(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_mtu(int mtu, std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_ipv4(const std::string& address, std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void enable_default_route(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void disable_default_route(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_default_route(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    std::string ipv4(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string ipv4_netmask(std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_ipv4_netmask(const std::string& netmask,
                          std::error_code& error) const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        error = std::make_error_code(std::errc::not_supported);
    }

    void disable_log_stdout() const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        throw std::runtime_error("not supported");
    }

    void enable_log_stdout() const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        throw std::runtime_error("not supported");
    }

    bool is_log_enabled() const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        throw std::runtime_error("not supported");
        return false;
    }

    int native_handle() const
    {
        Super::do_log(log_level::state, log_kind::unsupported_platform,
                      "Platform not supported");
        throw std::runtime_error("not supported");
        return -1;
    }
};
}
}
