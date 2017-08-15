// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/some.hpp>
#include <gtest/gtest.h>

TEST(test_some, return_value_of_some_method)
{
    tunnel::some s;
    EXPECT_TRUE(s.some_method());
}
