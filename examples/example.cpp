// Copyright (c) 2017 Steinwurf ApS
// All Rights Reserved
//
// THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF STEINWURF
// The copyright notice above does not evidence any
// actual or intended publication of such source code.

#include <tunnel/some.hpp>

#include <iostream>

int main()
{
    tunnel::some s;

    if (s.some_method())
    {
        std::cout << "Hello tunnel!" << std::endl;
    }

    return 0;
}
