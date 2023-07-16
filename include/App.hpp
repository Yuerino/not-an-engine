#ifndef NOT_AN_ENGINE_APP_HPP
#define NOT_AN_ENGINE_APP_HPP

#include <vulkan/vulkan.hpp>

#include "GlfwApi.hpp"
#include "Window.hpp"
#include "graphic/Graphic.hpp"

namespace nae {

class App {
public:
    static constexpr vk::Extent2D WINDOW_EXTENT = {800, 600};
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
    Graphic graphic_;
};

} // namespace nae

#endif // NOT_AN_ENGINE_APP_HPP
