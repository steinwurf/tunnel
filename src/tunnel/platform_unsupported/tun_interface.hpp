
namespace tunnel
{
namespace platform_unsupported
{
struct tun_interface
{
    void create(const std::string& interface_name, std::error_code& error)
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }
    void rename(const std::string& interface_name, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }

    void set_owner(const std::string& owner, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }

    void set_group(const std::string& group, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }

    std::string owner(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
        return "";
    }

    std::string interface_name(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
        return "";
    }

    bool is_up(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
        return false;
    }

    bool is_down(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
        return false;
    }

    void up(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }

    void down(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }

    bool is_persistent(std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
        return false;
    }

    void set_persistent(bool persistent, std::error_code& error) const
    {
        error = std::make_error_code(std::errc::not_supported);
        throw error;
    }
}
}
}
