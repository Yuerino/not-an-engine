#pragma once

#include "Window.hpp"

namespace nae {
    class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        static constexpr const char *TITLE = "Vulkan window";

        void run();

    private:
        Window window_{WIDTH, HEIGHT, TITLE};
    };
}// namespace nae
