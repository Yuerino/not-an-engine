#include "core/App.hpp"

#include "core/GlfwApi.hpp"
#include "core/Time.hpp"

namespace nae {

App *App::pAppInstance_{nullptr};

App::App(AppConfig appConfig) : appConfig_{std::move(appConfig)} {
    assert(pAppInstance_ == nullptr && "App already exists!");
    pAppInstance_ = this;

    pGraphicContext_ = std::make_unique<GraphicContext>();

    pWindow_ = std::make_unique<Window>(vk::Extent2D{appConfig_.width, appConfig_.height}, appConfig_.title);

    pGraphicContext_->initDeviceAndSwapchain();

    pRenderer_ = std::make_unique<Renderer>();

    pGraphicContext_->createDepthAndFrameBuffers(pRenderer_->getPipeline().getRenderPass());
}

App::~App() {
    pGraphicContext_->getDevice().get().waitIdle();
}

void App::run() {
    lastFrameTime_ = Time::now();
    isRunning_ = true;

    while (isRunning_ && not pWindow_->shouldClose()) {
        glfwWrapper([]() { glfwPollEvents(); });

        if (not pRenderer_->beginFrame()) {
            resize();
            continue;
        }

        Time currentTime = Time::now();
        Time timestep = currentTime - lastFrameTime_;
        lastFrameTime_ = currentTime;

        for (auto &pScene: scenes_) {
            pScene->onUpdate(timestep);
        }

        if (not pRenderer_->endFrame() || pWindow_->isFramebufferResized()) {
            resize();
        }
    }
}

void App::close() noexcept {
    isRunning_ = false;
}

void App::resize() noexcept {
    pGraphicContext_->recreateSwapchain(*pWindow_, pRenderer_->getPipeline().getRenderPass());
    pWindow_->resetFramebufferResized();

    float aspect = pWindow_->getAspectRatio();
    for (auto &pScene: scenes_) {
        pScene->onResize(aspect);
    }
}

} // namespace nae
