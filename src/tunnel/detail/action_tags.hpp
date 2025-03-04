// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#ifdef ACTION_TYPE_TAG
ACTION_TYPE_TAG(initialized, "Log system initialized.")

ACTION_TYPE_TAG(interface_created, "Creation of an interface.")
ACTION_TYPE_TAG(interface_renamed, "Renaming of an interface.")
ACTION_TYPE_TAG(set_owner, "Setting the owner.")
ACTION_TYPE_TAG(set_group, "Setting the group.")
ACTION_TYPE_TAG(owner, "Getting the owner.")
ACTION_TYPE_TAG(group, "Getting the group.")
ACTION_TYPE_TAG(interface_name, "Getting the interface name.")
ACTION_TYPE_TAG(native_handle, "Getting the native handle.")
ACTION_TYPE_TAG(is_up, "Checking if the interface is up.")
ACTION_TYPE_TAG(interface_up, "Bringing the interface up.")
ACTION_TYPE_TAG(is_down, "Checking if the interface is down.")
ACTION_TYPE_TAG(interface_down, "Bringing the interface down.")
ACTION_TYPE_TAG(interface_is_persistent,
                "Checking if the interface is persistent.")
ACTION_TYPE_TAG(set_persistent, "Setting the interface to be persistent.")
ACTION_TYPE_TAG(set_non_persistent,
                "Setting the interface to be non-persistent.")
ACTION_TYPE_TAG(interface_mtu, "Getting the interface MTU.")
ACTION_TYPE_TAG(set_mtu, "Setting the interface MTU.")
ACTION_TYPE_TAG(is_default_route,
                "Checking if the interface is the default route.")
ACTION_TYPE_TAG(enable_default_route,
                "Enabling the interface as the default route.")
ACTION_TYPE_TAG(disable_default_route,
                "Disabling the interface as the default route.")
ACTION_TYPE_TAG(ipv4, "Getting the interface IPv4 address.")
ACTION_TYPE_TAG(ipv4_netmask, "Getting the interface IPv4 netmask.")
ACTION_TYPE_TAG(set_ipv4, "Setting the interface IPv4 address.")
ACTION_TYPE_TAG(set_ipv4_netmask, "Setting the interface IPv4 netmask.")
ACTION_TYPE_TAG(make_sockaddr, "Making the socket address.")
ACTION_TYPE_TAG(send_netlink, "Sending a Netlink message.")
ACTION_TYPE_TAG(recv_netlink_message, "Receiving a Netlink message.")

/// Linux specific actions
ACTION_TYPE_TAG(open, "Opening a resource.")
ACTION_TYPE_TAG(socket, "Creating a socket.")
ACTION_TYPE_TAG(ioctl, "Performing an ioctl operation.")
ACTION_TYPE_TAG(send, "Sending data.")
ACTION_TYPE_TAG(recv, "Receiving data.")
ACTION_TYPE_TAG(bind, "Binding a socket.")
ACTION_TYPE_TAG(size, "Retrieving size information.")
ACTION_TYPE_TAG(read, "Reading data.")

/// Unsupported platform action
ACTION_TYPE_TAG(unsupported_platform,
                "Operation not supported on this platform.")
#else
#error "Missing ACTION_TYPE_TAG"
#endif
