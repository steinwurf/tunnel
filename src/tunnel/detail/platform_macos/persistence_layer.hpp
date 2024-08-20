#include <iostream>

// Persistence Layer
template <typename T>
struct persistence_layer : public T {
    persistence_layer() { std::cout << "persistence_layer created\n"; }

    void set_persistent(std::error_code& error) {
        std::cout << "Setting TUN interface to persistent\n";
    }

    void set_non_persistent(std::error_code& error) {
        std::cout << "Setting TUN interface to non-persistent\n";
    }
    
    bool is_persistent(std::error_code& error) const {
        // Implementation to check if persistent
        return false;
    }
};
