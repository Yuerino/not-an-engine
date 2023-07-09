#pragma once

#include <memory>

#include "Window.hpp"

namespace nae {
    class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        static constexpr const char *TITLE = "Vulkan window";

        App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;
        App(App &&) = delete;
        App &operator=(App &&) = delete;

        void run();

    private:
        Window window_;
    };
}// namespace nae
