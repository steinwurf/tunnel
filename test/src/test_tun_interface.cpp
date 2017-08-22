// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/tun_interface.hpp>
#include <gtest/gtest.h>

#include <boost/asio.hpp>

TEST(test_tun_interface, construct_no_su_fail)
{
    boost::asio::io_service io;
    std::error_code error;
    std::string name("testtun");

    // This should fail as super user permissions are needed by defaul
    auto t = tunnel::tun_interface::make_tun_interface(io, name, error);
    ASSERT_TRUE(bool(error));
}
