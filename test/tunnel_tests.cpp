// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.
#include "google/protobuf/stubs/common.h"
#include <cstdint>
#include <ctime>

#include <gtest/gtest.h>

GTEST_API_ int main(int argc, char** argv)
{
    srand(static_cast<uint32_t>(time(0)));

    testing::InitGoogleTest(&argc, argv);
    auto result = RUN_ALL_TESTS();

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
    return result;
}
