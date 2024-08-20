#include <iostream>
#include <string>
#include <system_error>

// Error Handling Layer
template <typename T>
struct error_handling_layer : public T {
    error_handling_layer() { std::cout << "error_handling_layer created\n"; }

    void handle_error(const std::string& msg) {
        std::cout << "Handling error: " << msg << "\n";
    }

    std::error_code translate_errno_to_error_code(int err) {
        std::cout << "Translating errno " << err << " to std::error_code\n";
        return std::error_code(err, std::generic_category());
    }
};