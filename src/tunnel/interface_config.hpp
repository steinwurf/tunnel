#pragma once
#include <platform/config.hpp>
#include <string>

#if defined(PLATFORM_LINUX)
struct config
{
    /// The name of the interface
    std::string interface_name;

    /// If true the interface will be created with Virtual Network
    /// Device Header enabled.
    bool vnet_hdr = false;

    /// If true the interface will be created with IFF_NO_PI enabled.
    bool iff_no_pi = true;
};
#elif defined(PLATFORM_MAC)
struct config
{
    // On MacOS we don't have any specific configuration (setting the name on macos is not possible)
};

#else
struct config
{
    /// The name of the interface
    std::string interface_name;

};

#endif
