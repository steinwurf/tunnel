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
endif()

# Include common CMake settings
include("${STEINWURF_RESOLVE}/toolchains/common_settings.cmake")

# platform dependency
if(NOT TARGET steinwurf::platform)
    add_subdirectory("${STEINWURF_RESOLVE}/platform" platform EXCLUDE_FROM_ALL)
endif()

# asio dependency add_compile_options(-DASIO_HAS_IO_URING)
if (NOT TARGET steinwurf::asio)
    add_subdirectory("${STEINWURF_RESOLVE}/asio" asio EXCLUDE_FROM_ALL)
endif ()


# poke dependency
if(NOT TARGET steinwurf::poke)
    add_subdirectory("${STEINWURF_RESOLVE}/poke" poke EXCLUDE_FROM_ALL)
endif()

# bourne dependency
if(NOT TARGET steinwurf::bourne)
    add_subdirectory("${STEINWURF_RESOLVE}/bourne" bourne EXCLUDE_FROM_ALL)
endif()



# Define library
file(GLOB_RECURSE tunnel_sources ./src/*.cpp)

# Is this the top-level steinwurf project?
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
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

find_package(Threads REQUIRED)

target_link_libraries(tunnel PUBLIC Threads::Threads)

# Link header only dependencies
target_link_libraries(tunnel PRIVATE platform)
target_link_libraries(tunnel PRIVATE poke)
target_link_libraries(tunnel PRIVATE bourne)
target_link_libraries(tunnel PRIVATE abacus)


# Set include directories
target_include_directories(tunnel INTERFACE src)

# Set properties
target_compile_features(tunnel PUBLIC cxx_std_17)

# Add alias for tunnel
add_library(steinwurf::tunnel ALIAS tunnel)

# Install headers excluding "detail" as these are internal to the library.
install(
    DIRECTORY ./src/tunnel
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    FILES_MATCHING
    PATTERN *.hpp
    PATTERN ./src/tunnel/detail EXCLUDE)

# Is top level project?
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    if (UNIX)
	  if(UNIX AND NOT APPLE)
		set(LINUX TRUE)
	  else()
		set(MACOS TRUE)
	  endif()

	  # This is not a lib dependency but only for the app and should not be included onless build is toplevel
    add_subdirectory("${STEINWURF_RESOLVE}/cli11-source" CLI11 EXCLUDE_FROM_ALL)

    enable_testing()

    if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
        # For Windows: Prevent overriding the parent project's compiler/linker
        # settings
        set(gtest_force_shared_crt
                ON
                CACHE BOOL "" FORCE)
    endif ()

    # Google Test dependency
    add_subdirectory("${STEINWURF_RESOLVE}/gtest")

    # Define library
    file(GLOB_RECURSE tunnel_test_sources test/**/*.cpp)
    list(APPEND tunnel_test_sources test/tunnel_tests.cpp)
    list(FILTER tunnel_test_sources EXCLUDE REGEX "[.]*\/platform[.]*")

	# Linux-specific code
	if (LINUX)
		file(GLOB_RECURSE linux_sources test/src/detail/platform_linux/*.cpp)
		list(APPEND tunnel_test_sources ${linux_sources})
	endif ()
	# MacOS-specific code
	if (MACOS)
		file(GLOB_RECURSE darwin_sources test/src/detail/platform_macos/*.cpp)
		list(APPEND tunnel_test_sources ${darwin_sources})
	endif ()



    # Build test executable
    add_executable(tunnel_tests ${tunnel_test_sources})
    target_link_libraries(tunnel_tests steinwurf::gtest)
    target_link_libraries(tunnel_tests steinwurf::tunnel)
	target_link_libraries(tunnel_tests steinwurf::platform)
	target_link_libraries(tunnel_tests steinwurf::poke)
	target_link_libraries(tunnel_tests steinwurf::bourne)
	target_link_libraries(tunnel_tests steinwurf::abacus)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(tunnel_tests stdc++fs)
    endif ()

    # Make sure we compile with C++17 and do not use compiler-specific extensions
    set_property(TARGET tunnel_tests PROPERTY CXX_STANDARD 17)
    set_property(TARGET tunnel_tests PROPERTY CXX_EXTENSIONS OFF)

    add_test(tunnel_tests tunnel_tests)

    add_subdirectory("apps/app")
	add_subdirectory("examples")
	endif ()
endif ()
