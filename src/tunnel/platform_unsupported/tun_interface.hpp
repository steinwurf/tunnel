
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
}
}
}
