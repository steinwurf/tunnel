// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#pragma once

#include <memory>
#include <string>
#include <system_error>

namespace tunnel
{
class tun_interface
{
public:
    /// Constructor
    tun_interface();
    tun_interface(tun_interface&&);
    tun_interface& operator=(tun_interface&&);

    /// Destructor
    ~tun_interface();

    /// Create the interface
    void create();
    void create(std::error_code& error);

    /// Create the interface with the specified name
    void create(const std::string& interface_name);
    void create(const std::string& interface_name, std::error_code& error);

    /// Rename the interface
    void rename(const std::string& interface_name) const;
    void rename(const std::string& interface_name, std::error_code& error) const;

    /// Set the owner of the interface
    void set_owner(const std::string& owner) const;
    void set_owner(const std::string& owner, std::error_code& error) const;

    /// Set the group the interface belongs to
    void set_group(const std::string& group) const;
    void set_group(const std::string& group, std::error_code& error) const;

    /// @return The owner of the interface or an empty string if no
    ///         is specified.
    std::string owner() const;
    std::string owner(std::error_code& error) const;

    /// @return The group of the interface belongs to or an empty
    ///         string if no group is specified.
    std::string group() const;
    std::string group(std::error_code& error) const;

    /// @return The interface name
    std::string interface_name() const;
    std::string interface_name(std::error_code& error) const;

    /// @return The native file descriptor of the TUN interface. This
    ///         can be used for reading and writing data to and from
    ///         the interface.
    int native_handle() const;

    /// @return True if the interface is up
    bool is_up() const;
    bool is_up(std::error_code& error) const;

    /// Set the interface up
    void up() const;
    void up(std::error_code& error) const;

    /// @return True if the interface is down
    bool is_down() const;
    bool is_down(std::error_code& error) const;

    /// Set the interface down
    void down() const;
    void down(std::error_code& error) const;

    /// @return true if the interface is persistent
    bool is_persistent() const;
    bool is_persistent(std::error_code& error) const;

    /// Change the persistent status - if persistent the interface
    /// will not disappear when the application closes.
    void set_persistent();
    void set_persistent(std::error_code& error);

    /// Set the interface non persistant i.e. the interface will only
    /// be around for as long as the application keeps it alive.
    void set_non_persistent();
    void set_non_persistent(std::error_code& error);

    /// @return The MTU (Maximum Transfer Unit) of the interface
    uint32_t mtu() const;
    uint32_t mtu(std::error_code& error) const;

    /// Set the MTU (Maximum Transfer Unit) of the interface
    void set_mtu(uint32_t mtu) const;
    void set_mtu(uint32_t mtu, std::error_code& error) const;

    /// Check if the interface is the default route
    bool is_default_route() const;
    bool is_default_route(std::error_code& error) const;

    /// Enable default route for this interface
    void enable_default_route() const;
    void enable_default_route(std::error_code& error) const;

    /// Disable default route for this interface
    void disable_default_route() const;
    void disable_default_route(std::error_code& error) const;

    /// @return The IPv4 address for the interface
    std::string ipv4() const;
    std::string ipv4(std::error_code& error) const;

    /// @return The IPv4 netmask of the interface
    std::string ipv4_netmask() const;
    std::string ipv4_netmask(std::error_code& error) const;

    /// Set the IPv4 address of the interface.
    void set_ipv4(const std::string& ip) const;
    void set_ipv4(const std::string& ip, std::error_code& error) const;

    /// Set the IPv4 netmask
    void set_ipv4_netmask(const std::string& mask) const;
    void set_ipv4_netmask(const std::string& mask,
                          std::error_code& error) const;

    /// Enable printing log information to stdout
    void enable_log_stdout();

    /// Disable printing log information to stdout
    void disable_log_stdout();

    /// Check if printing log information to stdout
    bool is_log_enabled() const;

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};
}
