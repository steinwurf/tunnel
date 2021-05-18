// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <gtest/gtest.h>
#include <tunnel/tun_interface.hpp>

TEST(test_tun_interface, construct_no_su_fail)
{
    std::error_code error;
    tunnel::tun_interface t;
    // This should fail as super user permissions are needed by default
    t.create("", error);
    ASSERT_TRUE(bool(error));
}
