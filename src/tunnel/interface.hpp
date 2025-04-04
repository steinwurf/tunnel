// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <memory>
#include <platform/config.hpp>
#include <string>
#include <system_error>

#include "monitor.hpp"

namespace tunnel
{
class interface
{
public:
    enum class type
    {
        tun,
        tap
    };

#if defined(PLATFORM_LINUX)
    struct config
    {
        /// The mode of the interface
        type interface_type = type::tun;

        /// The name of the interface
        std::string interface_name;

        /// If true the interface will be created with
        /// Virtual Network Device Header enabled.
        bool vnet_hdr = false;

        /// If true the interface will be created with IFF_NO_PI enabled.
        bool iff_no_pi = true;
    };
#elif defined(PLATFORM_MAC)
    struct config
    {
        /// The mode of the interface
        type interface_type = type::tun;

        // On MacOS we don't have any specific configuration (setting the name
        // on macos is not possible)
    };
#else
    struct config
    {
        /// The mode of the interface
        type interface_type;
    };
#endif
public:
    /// Constructor
    interface();
    interface(interface&&);
    interface& operator=(interface&&);

    /// Destructor
    ~interface();

    /// Create the interface
    void create(const interface::config& config);

    /// Create the interface
    /// @param the config for the interface
    void create(const interface::config& config, std::error_code& error);

    /// Rename the interface
    /// @param interface_name The new name of the interface
    void rename(const std::string& interface_name) const;

    /// Rename the interface
    /// @param interface_name The new name of the interface
    /// @param error The error code in case of failure
    void rename(const std::string& interface_name,
                std::error_code& error) const;

    /// Set the owner of the interface
    /// @param owner The owner of the interface
    void set_owner(const std::string& owner) const;

    /// Set the owner of the interface
    /// @param owner The owner of the interface
    /// @param error The error code in case of failure
    void set_owner(const std::string& owner, std::error_code& error) const;

    /// Set the group the interface belongs to
    /// @param group The group the interface belongs to
    void set_group(const std::string& group) const;

    /// Set the group the interface belongs to
    /// @param group The group the interface belongs to
    /// @param error The error code in case of failure
    void set_group(const std::string& group, std::error_code& error) const;

    /// @return The owner of the interface or an empty string if no
    ///         is specified.
    auto owner() const -> std::string;

    /// @return The owner of the interface or an empty string if no
    ///         is specified.
    /// @param error The error code in case of failure
    auto owner(std::error_code& error) const -> std::string;

    /// @return The group of the interface belongs to or an empty
    ///         string if no group is specified.
    auto group() const -> std::string;

    /// @return The group of the interface belongs to or an empty
    ///         string if no group is specified.
    /// @param error The error code in case of failure
    auto group(std::error_code& error) const -> std::string;

    /// @return The interface name
    auto interface_name() const -> std::string;

    /// @return The interface name
    /// @param error The error code in case of failure
    auto interface_name(std::error_code& error) const -> std::string;

    /// @return The native file descriptor of the interface. This
    ///         can be used for reading and writing data to and from
    ///         the interface.
    auto native_handle() const -> int;

    /// @return True if the interface is up
    auto is_up() const -> bool;

    /// @return True if the interface is up
    /// @param error The error code in case of failure
    auto is_up(std::error_code& error) const -> bool;

    /// Set the interface up
    void up() const;

    /// Set the interface up
    /// @param error The error code in case of failure
    void up(std::error_code& error) const;

    /// @return True if the interface is down
    auto is_down() const -> bool;

    /// @return True if the interface is down
    /// @param error The error code in case of failure
    auto is_down(std::error_code& error) const -> bool;

    /// Set the interface down
    void down() const;

    /// Set the interface down
    /// @param error The error code in case of failure
    void down(std::error_code& error) const;

    /// @return true if the interface is persistent
    auto is_persistent() const -> bool;

    /// @return true if the interface is persistent
    /// @param error The error code in case of failure
    auto is_persistent(std::error_code& error) const -> bool;

    /// Change the persistent status - if persistent the interface
    /// will not disappear when the application closes.
    void set_persistent();

    /// Change the persistent status - if persistent the interface
    /// will not disappear when the application closes.
    /// @param error The error code in case of failure
    void set_persistent(std::error_code& error);

    /// Set the interface non-persistent, i.e., the interface will only
    /// be around for as long as the application keeps it alive.
    void set_non_persistent();

    /// Set the interface non-persistent, i.e., the interface will only
    /// be around for as long as the application keeps it alive.
    /// @param error The error code in case of failure
    void set_non_persistent(std::error_code& error);

    /// @return The MTU (Maximum Transfer Unit) of the interface
    auto mtu() const -> uint32_t;

    /// @return The MTU (Maximum Transfer Unit) of the interface
    /// @param error The error code in case of failure
    auto mtu(std::error_code& error) const -> uint32_t;

    /// Set the MTU (Maximum Transfer Unit) of the interface
    /// @param mtu The new MTU
    void set_mtu(uint32_t mtu) const;

    /// Set the MTU (Maximum Transfer Unit) of the interface
    /// @param mtu The new MTU
    /// @param error The error code in case of failure
    void set_mtu(uint32_t mtu, std::error_code& error) const;

    /// Check if the interface is the default route
    auto is_default_route() const -> bool;

    /// Check if the interface is the default route
    /// @param error The error code in case of failure
    auto is_default_route(std::error_code& error) const -> bool;

    /// Enable default route for this interface
    void enable_default_route() const;

    /// Enable default route for this interface
    /// @param error The error code in case of failure
    void enable_default_route(std::error_code& error) const;

    /// Disable default route for this interface
    void disable_default_route() const;

    /// Disable default route for this interface
    /// @param error The error code in case of failure
    void disable_default_route(std::error_code& error) const;

    /// @return The IPv4 address for the interface
    auto ipv4() const -> std::string;

    /// @return The IPv4 address for the interface
    /// @param error The error code in case of failure
    auto ipv4(std::error_code& error) const -> std::string;

    /// @return The IPv4 netmask of the interface
    auto ipv4_netmask() const -> std::string;

    /// @return The IPv4 netmask of the interface
    /// @param error The error code in case of failure
    auto ipv4_netmask(std::error_code& error) const -> std::string;

    /// Set the IPv4 address of the interface.
    /// @param ip The new IPv4 address
    void set_ipv4(const std::string& ip) const;

    /// Set the IPv4 address of the interface.
    /// @param ip The new IPv4 address
    /// @param error The error code in case of failure
    void set_ipv4(const std::string& ip, std::error_code& error) const;

    /// Set the IPv4 netmask
    /// @param mask The new IPv4 netmask
    void set_ipv4_netmask(const std::string& mask) const;

    /// Set the IPv4 netmask
    /// @param mask The new IPv4 netmask
    /// @param error The error code in case of failure
    void set_ipv4_netmask(const std::string& mask,
                          std::error_code& error) const;

    /// @return The monitor for the interface
    auto monitor() const -> const tunnel::monitor&;

    /// @return The monitor for the interface
    auto monitor() -> tunnel::monitor&;

    /// Return true if the platform is supported, otherwise false
    static auto is_platform_supported() -> bool;

private:
    struct impl;
    std::unique_ptr<impl> m_impl;
};
}
