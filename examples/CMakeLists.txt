cmake_minimum_required(VERSION 3.12)
project(tunnel_examples)

add_executable(interface interface.cpp)
target_link_libraries(interface steinwurf::tunnel)

add_executable(read_data read_data.cpp)
target_link_libraries(read_data steinwurf::tunnel)

add_executable(sample_tunnel sample_tunnel.cpp)
target_link_libraries(sample_tunnel steinwurf::tunnel)
target_link_libraries(sample_tunnel steinwurf::asio)
target_link_libraries(sample_tunnel CLI11)

add_executable(backtobacktun backtobacktun.cpp)
target_link_libraries(backtobacktun steinwurf::tunnel)

target_compile_features(interface PUBLIC cxx_std_17)
target_compile_features(read_data PUBLIC cxx_std_17)
target_compile_features(sample_tunnel PUBLIC cxx_std_17)
target_compile_features(backtobacktun PUBLIC cxx_std_17)

set_property(TARGET interface PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET read_data PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET sample_tunnel PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET backtobacktun PROPERTY CXX_EXTENSIONS OFF)



if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(tunnel_app stdc++fs)
    add_compile_definitions(CLI11_HAS_FILESYSTEM=0)
endif ()


# Enable pedantic errors
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC is throwing ISO errors from abseil that we can't fix
else ()
    target_compile_options(tunnel_app PUBLIC -pedantic-errors)
endif ()
