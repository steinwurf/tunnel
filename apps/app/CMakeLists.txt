cmake_minimum_required(VERSION 3.12)
project(tunnel_app)

# App executable
file(GLOB_RECURSE tunnel_app_sources ./*.cpp)

add_executable(tunnel_app ${tunnel_app_sources})

target_compile_features(tunnel_app PUBLIC cxx_std_14)

target_link_libraries(tunnel_app steinwurf::tunnel)
target_link_libraries(tunnel_app steinwurf::asio)
target_link_libraries(tunnel_app steinwurf::poke)
target_link_libraries(tunnel_app steinwurf::bourne)
target_link_libraries(tunnel_app CLI11)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(tunnel_app stdc++fs)
    add_compile_definitions(CLI11_HAS_FILESYSTEM=0)
endif ()

# Make sure we compile with C++17 and do not use compiler specific extensions
set_property(TARGET tunnel_app PROPERTY CXX_STANDARD 17)
set_property(TARGET tunnel_app PROPERTY CXX_EXTENSIONS OFF)

# Enable pedantic errors
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC is throwing ISO errors from abseil that we can't fix
else ()
    target_compile_options(tunnel_app PUBLIC -pedantic-errors)
endif ()
