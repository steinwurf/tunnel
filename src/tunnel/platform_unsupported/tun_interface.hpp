
namespace tunnel
{
namespace platform_unsupported
{
struct tun_interface
{
    void create(const std::string& interface_name, std::error_code& error)
    {
        error = std::make_error_code(std::errc::not_supported);
    }
    void rename(const std::string& interface_name, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_owner(const std::string& owner, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string& group, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string interface_name(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    bool is_up(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    bool is_down(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void up(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void down(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    int mtu(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return 0;
    }

    void set_non_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_mtu(int mtu, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_ipv4(const std::string& address, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void enable_default_route(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void disable_default_route(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_default_route(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    std::string ipv4(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string ipv4_netmask(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_ipv4_netmask(const std::string& netmask,
                          std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
    }

    void disable_log_stdout() const
    {
        throw std::runtime_error("not supported");
    }

    void enable_log_stdout() const
    {
        throw std::runtime_error("not supported");
    }

    bool is_log_enabled() const
    {
        throw std::runtime_error("not supported");
        return false;
    }

    int native_handle() const
    {
        throw std::runtime_error("not supported");
        return -1;
    }

    static bool is_platform_supported()
    {
        return false;
    }
};
}
}
