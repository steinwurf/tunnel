News for tunnel
===============

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* tbd

4.1.0
-----
* Minor: Upgrade to boost 4

4.0.0
-----
* Major: Refactor project with an all new API.
* Major: Removed external boost dependency.
* Major: Added platform dependency.

3.0.0
-----
* Major: Wanted device name must now be less than ``IFNAMSIZ-1``.
* Minor: Added ``ipv4`` and ``is_default_route_enabled``.
* Major: Renamed ``set_default_route`` -> ``enable_default_route`` and
  ``remove_default_route`` -> ``disable_default_route``.

2.0.0
-----
* Major: Removed a couple of abbreviations.
* Major: Changed API to take pointer and size instead of std::vector.
* Patch: Fixed make_tun_interface so that it now takes a const string reference
  to the dev name.
* Major: Removed dependency on Links.
* Minor: Added uint32_t mtu() const.

1.1.0
-----
* Minor: Added set_mtu on tun_interface

1.0.1
------
* Patch: waf now only builds project for linux platforms

1.0.0
-----
* Major: Added tun_interface class
* Major: Added nanonet scripts for quick testing
* Minor: Added tunnel example
