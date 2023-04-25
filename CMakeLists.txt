cmake_minimum_required(VERSION 3.12)
project(tunnel)

find_package(Python COMPONENTS Interpreter)

# Use waf to resolve dependencies
if(NOT DEFINED STEINWURF_RESOLVE)
    message(STATUS "Resolving dependencies...")
    execute_process(
        COMMAND ${Python_EXECUTABLE} waf resolve ${STEINWURF_RESOLVE_OPTIONS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE STATUS)

    if(STATUS AND NOT STATUS EQUAL 0)
        message(FATAL_ERROR "Failed: ${STATUS}")
    endif()

    set(STEINWURF_RESOLVE "${CMAKE_CURRENT_SOURCE_DIR}/resolve_symlinks")
    set(STEINWURF_TOP_NAME tunnel)
endif()

# platform dependency
if(NOT TARGET steinwurf::platform)
    add_subdirectory("${STEINWURF_RESOLVE}/platform" platform EXCLUDE_FROM_ALL)
endif()

# poke dependency
if(NOT TARGET steinwurf::poke)
    add_subdirectory("${STEINWURF_RESOLVE}/poke" poke EXCLUDE_FROM_ALL)
endif()

# Define library
file(GLOB_RECURSE tunnel_sources ./src/*.cpp)

# Is this the top-level steinwurf project?
if(${PROJECT_NAME} STREQUAL ${STEINWURF_TOP_NAME})
    # Create static library
    add_library(tunnel STATIC ${tunnel_sources})

    # Install library
    install(FILES $<TARGET_FILE:tunnel> DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else()
    # Create object library
    add_library(tunnel OBJECT ${tunnel_sources})

    # Add this library to a global list of steinwurf object libraries
    set_property(GLOBAL APPEND PROPERTY steinwurf::object_libraries
        steinwurf::tunnel)
endif()

# Link header only dependencies
target_link_libraries(tunnel PRIVATE platform)
target_link_libraries(tunnel PRIVATE poke)

# Set include directories
target_include_directories(tunnel INTERFACE src)

# Set properties
target_compile_features(tunnel PUBLIC cxx_std_14)

# Add alias for tunnel
add_library(steinwurf::tunnel ALIAS tunnel)

# Install headers excluding "detail" as these are internal to the library.
install(
    DIRECTORY ./src/tunnel
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    FILES_MATCHING
    PATTERN *.hpp
    PATTERN ./src/tunnel/detail EXCLUDE)
