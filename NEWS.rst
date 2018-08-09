News for tunnel
===============

This file lists the major changes between versions. For a more detailed list of
every change, see the Git log.

Latest
------
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
