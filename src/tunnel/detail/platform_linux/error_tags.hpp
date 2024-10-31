// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#ifdef ERROR_TAG
ERROR_TAG(no_error, "No error.")
ERROR_TAG(interface_name_too_long,
          "The interface name is too long, max size is IFNAMSIZ")
ERROR_TAG(mtu_too_large, "The MTU value specified is too large.")
ERROR_TAG(nlmsg_ok, "Got false from NLMSG_OK macro.")
ERROR_TAG(nlmsg_error, "Got a NLMSG_ERROR.")
ERROR_TAG(invalid_interface_type, "Invalid interface type.")
#else
#error "Missing ERROR_TAG"
#endif
