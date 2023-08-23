#include "core/App.hpp"

#include "core/GlfwApi.hpp"
#include "core/Time.hpp"

namespace nae {

App::App(AppConfig appConfig)
    : appConfig_{std::move(appConfig)},
      graphicInstance_{{}, GraphicInstance::getGlfwRequiredExtensions()},
      window_{{appConfig_.width, appConfig_.height}, appConfig_.title} {
    graphicInstance_.initDeviceAndSwapchain(window_);
    pRenderer_ = std::make_unique<Renderer>(graphicInstance_);
    graphicInstance_.createDepthAndFrameBuffers(pRenderer_->getPipeline().getRenderPass());
}

App::~App() {
    graphicInstance_.getDevice().get().waitIdle();
}

void App::addScene(std::unique_ptr<Scene> pScene) {
    scenes_.emplace_back(std::move(pScene));
    scenes_.back()->onAttach(*pRenderer_);
}

void App::run() {
    lastFrameTime_ = Time::now();
    isRunning_ = true;

    while (isRunning_ && not window_.shouldClose()) {
        glfwWrapper([]() { glfwPollEvents(); });

        if (not pRenderer_->beginFrame()) {
            graphicInstance_.recreateSwapchain(window_, pRenderer_->getPipeline().getRenderPass());
            continue;
        }

        Time currentTime = Time::now();
        Time timestep{currentTime.getSeconds() - lastFrameTime_.getSeconds()};
        lastFrameTime_ = currentTime;

        for (auto &pScene: scenes_) {
            pScene->onUpdate(timestep);
        }

        if (not pRenderer_->endFrame() || window_.isFramebufferResized()) {
            graphicInstance_.recreateSwapchain(window_, pRenderer_->getPipeline().getRenderPass());
            window_.resetFramebufferResized();
        }
    }
}

void App::close() noexcept {
    isRunning_ = false;
}

} // namespace nae
