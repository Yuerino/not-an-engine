#pragma once

#include <cstdint>
#include <string>

#include "core/GlfwApi.hpp"
#include "core/GraphicContext.hpp"
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
    ~App();

    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;
    App &operator=(App &&) = delete;

    void addScene(std::unique_ptr<Scene> pScene);

    void run();
    void close() noexcept;

    [[nodiscard]] static App &get() noexcept { return *pAppInstance_; }
    [[nodiscard]] GraphicContext &getGraphicContext() noexcept { return *pGraphicContext_; }
    [[nodiscard]] Window &getWindow() noexcept { return *pWindow_; }
    [[nodiscard]] Renderer &getRenderer() noexcept { return *pRenderer_; }

private:
    AppConfig appConfig_;

    std::unique_ptr<GraphicContext> pGraphicContext_;
    std::unique_ptr<Window> pWindow_;
    std::unique_ptr<Renderer> pRenderer_;

    std::vector<std::unique_ptr<Scene>> scenes_;

    bool isRunning_{false};
    Time lastFrameTime_{0};

    static App *pAppInstance_;
};

} // namespace nae
