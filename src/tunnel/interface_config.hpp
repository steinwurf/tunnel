#pragma once
#include <string>
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
