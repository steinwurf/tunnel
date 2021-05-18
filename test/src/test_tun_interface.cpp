// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#include <gtest/gtest.h>
#include <tunnel/tun_interface.hpp>

TEST(test_tun_interface, construct)
{
    std::error_code error;
    tunnel::tun_interface t;
    (void)t;
}
