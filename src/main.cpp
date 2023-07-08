#include "App.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    nae::App app{};

    try {
        app.run();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
