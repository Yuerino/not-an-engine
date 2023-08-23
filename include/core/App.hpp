#pragma once

#include <cstdint>
#include <string>

#include "core/GlfwApi.hpp"
#include "core/GraphicInstance.hpp"
#include "core/Scene.hpp"
#include "core/Window.hpp"
#include "renderer/Renderer.hpp"

namespace nae {

struct AppConfig {
    uint32_t width{800};
    uint32_t height{600};
    std::string title{"Vulkan window"};
};

class App {
public:
    explicit App(AppConfig appConfig = {});
    virtual ~App();

    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

    void addScene(std::unique_ptr<Scene> pScene);

    void run();
    void close() noexcept;

private:
    AppConfig appConfig_;

    GlfwApi glfwApi_{};
    GraphicInstance graphicInstance_;
    Window window_;
    std::unique_ptr<Renderer> pRenderer_;

    std::vector<std::unique_ptr<Scene>> scenes_;

    bool isRunning_{false};
    Time lastFrameTime_{0};
};

} // namespace nae
