#ifndef NOT_AN_ENGINE_APP_HPP
#define NOT_AN_ENGINE_APP_HPP

#include "Device.hpp"
#include "GlfwApi.hpp"
#include "Window.hpp"

namespace nae {

class App {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr const char *TITLE = "Vulkan window";

    App();
    ~App() = default;

    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

    void run();

private:
    GlfwApi glfwApi_;
    Window window_;
    Device device_;
};

} // namespace nae

#endif // NOT_AN_ENGINE_APP_HPP
