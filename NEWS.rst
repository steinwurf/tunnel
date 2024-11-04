News for tunnel
===============

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
* Major: Merged the tun_interface and tap_interface classes into a single
  class called interface. The interface class can now create both TUN and TAP
  interfaces based on the interface_type parameter provided to the create methods
  config argument.

15.0.0
------
* Minor: Add initial macos/Darwin support.
* Major: Change interface from seperate args to a struct

14.1.1
------
* Patch: Only have CLI11 as a dependency if compilation is toplevel

14.1.0
------
* Minor: Add tap support and proper in repository testing 

14.0.0
------
* Major: Updated poke to version 13.

13.1.0
------
* Minor: Added documentation for all functions in the ``tun_interface`` class.
* Minor: Changed backend parameter order to always have the error code last.
* Patch: Removed duplicated code.

13.0.0
------
* Major: Updated poke to use version 12.

12.0.0
------
* Major: Updated poke to use version 11.

11.0.0
------
* Minor: Updated poke to use version 10.
* Major: Extended the log_callback to now also take a std::any user data argument.

10.0.2
------
* Patch: Upgraded poke to use 9.
* Patch: Upgraded abacus to use 6.

10.0.1
------
* Patch: Upgraded poke to use 8.0.0.

10.0.0
------
* Major: Update dependencies to use Poke 7.

9.0.1
-----
* Patch: Fixed a bug where insufficient permissions would cause the library to
  assert instead of throwing an exception when using the exception-based API.

9.0.0
-----
* Major: Use waf-tools 5.

8.1.0
-----
* Minor: Support for TSO and UFO on Linux.

8.0.0
-----
* Major: Upgrade to poke version 5.
* Major: Change structure so that details are now under ``detail`` namespace.

7.0.0
-----
* Major: Added a dependency on `poke`
* Minor: Added an empty Windows and Mac implementation of tun_interface.
  Will throw errors if used.

6.0.2
-----
* Patch: Renamed namespace `linux` to `platform_linux` to avoid conflicts with
  `linux` define set by gcc and clang.

6.0.1
------
* Patch: Removed check for OpenWRT as the library seems to compile with OpenWRT 21.02.0
* Patch: Removed an unnecessary header include causing redefinition of struct ethhdr

6.0.0
-----
* Major: Do not prepend a protocol information header, i.e.,
  add the IFF_NO_PI flag.

5.1.0
-----
* Minor: Added CMake support.
* Minor: Updated waf.

5.0.0
-----
* Major: Upgrade platform dependency.

4.1.1
-----
* Patch: Disable OpenWRT platform.

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
