#include <iostream>

// Resource Management Layer
template <typename T>
struct resource_management_layer : public T {
    resource_management_layer() { std::cout << "resource_management_layer created\n"; }

    void open() {
        std::cout << "Resource management: open called\n";
    }

    void close() {
        std::cout << "Resource management: close called\n";
    }

    void cleanup() {
        std::cout << "Resource management: cleanup called\n";
    }
};