// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cassert>
#include <unistd.h>

namespace tunnel
{
namespace detail
{
namespace platform_linux
{
/// RAII (Resource Allocation Is Initialization) wrapper for a file
/// descriptor. Ensures that the file descriptor gets closed.
// template<class ClosePolicy = close_policy>
class scoped_file_descriptor
{
public:
    /// Default constructor for a non-owning scoped_file_descriptor
    /// i.e. it will not do anything when going out of scope.
    scoped_file_descriptor() : m_file_descriptor(-1)
    {
    }

    /// Take ownership of a "naked" file descriptor
    ///
    /// @param file_descriptor The file descriptor we will own.
    scoped_file_descriptor(int file_descriptor) :
        m_file_descriptor(file_descriptor)
    {
        assert(m_file_descriptor >= 0);
    }

    /// Take ownership of a file descriptor wrapped in a temporary
    /// scoped_file_descriptor
    ///
    /// @param other Temporary scoped_file_descriptor where we will
    ///              take ownership of the contained file descriptor
    scoped_file_descriptor(scoped_file_descriptor&& other)
    {
        m_file_descriptor = other.m_file_descriptor;
        other.m_file_descriptor = -1;
    }

    /// Take ownership of a file descriptor wrapped in a temporary
    /// scoped_file_descriptor
    ///
    /// @param other Temporary scoped_file_descriptor where we will
    ///              take ownership of the contained file descriptor
    auto operator=(scoped_file_descriptor&& other) -> scoped_file_descriptor&
    {
        m_file_descriptor = other.m_file_descriptor;
        other.m_file_descriptor = -1;
        return *this;
    }

    /// Destructor which will ensure that an owned file descriptor
    /// will be closed.
    ~scoped_file_descriptor()
    {
        if (m_file_descriptor != -1)
        {
            // We ignore any errors that can occur here - no clue if we
            // can do something better?
            ::close(m_file_descriptor);
        }
    }

    /// @return The contained "naked" file descriptor.
    auto native_handle() const -> int
    {
        assert(m_file_descriptor != -1);
        return m_file_descriptor;
    }

    /// @return True if a valid file descriptor is contained otherwise
    ///         false.
    explicit operator bool() const
    {
        return m_file_descriptor != -1;
    }

private:
    /// Delete the copy and copy assignment constructors - we cannot
    /// have two scoped_file_descriptors owning the same file
    /// descriptor.
    scoped_file_descriptor(const scoped_file_descriptor&) = delete;

    auto operator=(const scoped_file_descriptor&)
        -> scoped_file_descriptor& = delete;

private:
    /// The owned file descriptor
    int m_file_descriptor;
};

}
}
}
