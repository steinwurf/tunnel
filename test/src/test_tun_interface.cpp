// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/tun_interface.hpp>
#include <gtest/gtest.h>

TEST(test_tun_interface, construct_no_su_fail)
{
    std::error_code error;
    tunnel::tun_interface t;
    // This should fail as super user permissions are needed by default
    t.create("", error);
    ASSERT_TRUE(bool(error));
}
