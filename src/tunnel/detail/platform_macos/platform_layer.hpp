#include <iostream>
#include "../../interface_config.hpp"// Platform Layer
template <typename T>
struct platform_layer : public T {
    platform_layer() { std::cout << "platform_layer created\n"; }

    void open_tun_device() {
        std::cout << "Platform-specific open_tun_device() called\n";
    }

    void configure_interface() {
        std::cout << "Platform-specific configure_interface() called\n";
    }

    void close_tun_device() {
        std::cout << "Platform-specific close_tun_device() called\n";
    }

    void create(const config& config, std::error_code& error) {
        // Platform-specific implementation
    }

    void rename(const std::string& interface_name, std::error_code& error) {
        // Platform-specific implementation
    }

    int native_handle() const {
        // Return platform-specific handle
        return 0;
    }

};