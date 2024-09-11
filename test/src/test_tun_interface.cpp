// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <gtest/gtest.h>
#include <platform/config.hpp>
#include <tunnel/tun_interface.hpp>

#if defined(PLATFORM_MAC)

TEST(test_tun_interface, construct_no_su_fail)
{
    tunnel::tun_interface t;
    (void)t;
}

TEST(test_tun_interface, create_no_su_expect_fail)
{
    // Check if we are root
    if (getuid() != 0)
    {
        tunnel::tun_interface t;
        EXPECT_THROW(t.create({}), std::system_error);
    }
}

#elif defined(PLATFORM_LINUX)
TEST(test_tun_interface, construct_no_su_fail)
{
    tunnel::tun_interface t;
    (void)t;
}

TEST(test_tun_interface, create_no_su_expect_fail)
{
    // Check if we are root
    if (getuid() != 0)
    {
        tunnel::tun_interface t;
        EXPECT_THROW(t.create({"dummy", false}), std::system_error);
        EXPECT_THROW(t.create({"dummy", true}), std::system_error);
        EXPECT_THROW(t.create({"too_long_name_way_over_20_chars", false}),
                     std::system_error);
    }
}
#endif
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
TEST(test_tun_interface, options)
{
    uid_t uid = getuid();
    if (uid != 0)
    {
        GTEST_SKIP();
    }
    const auto ip_addr = "192.168.60.1";
    const auto netmask = "255.255.0.0";
    const auto mtu = 1500;
    tunnel::tun_interface t;
    t.create({});
    t.up();
    t.set_mtu(1500);
    EXPECT_EQ(t.mtu(), mtu);
    t.set_ipv4(ip_addr);
    EXPECT_EQ(t.ipv4(), ip_addr);
    t.set_ipv4_netmask(netmask);
    EXPECT_EQ(t.ipv4_netmask(), netmask);
    t.enable_default_route();
    t.disable_default_route();
    t.down();
}
#endif
