// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <gtest/gtest.h>
#include <tunnel/tun_interface.hpp>
#include <unistd.h>

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
