#ifdef ERROR_TAG
ERROR_TAG(interface_name_too_long,
          "The interface name is too long, max size is IFNAMSIZ")
ERROR_TAG(mtu_too_large, "The MTU value specified is too large.")
ERROR_TAG(nlmsg_ok, "Got false from NLMSG_OK macro.")
ERROR_TAG(nlmsg_error, "Got a NLMSG_ERROR.")

#else
#error "Missing ERROR_TAG"
#endif
