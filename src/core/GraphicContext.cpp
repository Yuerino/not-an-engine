#include "core/GraphicContext.hpp"

#include <cassert>
#include <iostream>

#include "GLFW/glfw3.h"

#include "core/App.hpp"
#include "core/GlfwApi.hpp"

namespace nae {

static vk::DebugUtilsMessengerCreateInfoEXT makeDebugUtilsMessengerCreateInfoEXT();

GraphicContext::GraphicContext(const std::vector<std::string> &layers,
                               const std::vector<std::string> &extensions,
                               uint32_t apiVersion) {
    auto glfwRequiredExtensions = getGlfwRequiredExtensions();
    glfwRequiredExtensions.insert(glfwRequiredExtensions.end(), extensions.begin(), extensions.end());

    // TODO: log layers and extensions
    vk::ApplicationInfo appInfo{"Application made with not-an-engine",
                                VK_MAKE_API_VERSION(0, 1, 0, 0),
                                "not-an-engine",
                                VK_MAKE_API_VERSION(0, 1, 0, 0),
                                apiVersion};

#if defined(NDEBUG)
    gatherLayers(layers);
    gatherExtensions(glfwRequiredExtensions);

    vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfoChain{
            {vk::InstanceCreateFlags{}, &appInfo, enabledLayers_, enabledExtensions_}};
#else
    gatherLayers(layers, vkContext_.enumerateInstanceLayerProperties());
    gatherExtensions(glfwRequiredExtensions, vkContext_.enumerateInstanceExtensionProperties());

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instanceCreateInfoChain{
            {vk::InstanceCreateFlags{}, &appInfo, enabledLayers_, enabledExtensions_},
            makeDebugUtilsMessengerCreateInfoEXT()};
#endif

    vkInstance_ = vk::raii::Instance{vkContext_, instanceCreateInfoChain.get<vk::InstanceCreateInfo>()};

#if !defined(NDEBUG)
    vkDebugUtilsMessenger_ = vk::raii::DebugUtilsMessengerEXT{vkInstance_, makeDebugUtilsMessengerCreateInfoEXT()};
#endif
}

void GraphicContext::initDeviceAndSwapchain() {
    const Window &window = App::get().getWindow();

    VkSurfaceKHR vkSurfaceKHR{};
    auto result = glfwWrapper([&]() {
        return glfwCreateWindowSurface(static_cast<VkInstance>(*vkInstance_),
                                       static_cast<GLFWwindow *>(window),
                                       nullptr,
                                       &vkSurfaceKHR);
    });
    assert(result == VK_SUCCESS && "Failed to create window surface");
    vk::raii::SurfaceKHR vkSurface{vkInstance_, vkSurfaceKHR};
    pSurface_ = std::make_unique<Surface>(std::move(vkSurface));

    std::vector<std::string> pdExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    pPhysicalDevice_ = std::make_unique<PhysicalDevice>(*this, pdExtensions);
    pDevice_ = std::make_unique<Device>(*pPhysicalDevice_, *pSurface_);
    pSwapchain_ = std::make_unique<Swapchain>(
            *pPhysicalDevice_,
            *pDevice_,
            *pSurface_,
            window.getExtent(),
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc);
}

void GraphicContext::createDepthAndFrameBuffers(const RenderPass &renderPass) {
    pSwapchain_->createDepthBuffer(*pDevice_);
    pSwapchain_->createFrameBuffers(*pDevice_, renderPass);
}

void GraphicContext::recreateSwapchain(const Window &window, const RenderPass &renderPass) {
    auto windowExtent = window.getExtent();
    while (windowExtent.width == 0 || windowExtent.height == 0) {
        windowExtent = window.getExtent();
        glfwWrapper([]() { glfwWaitEvents(); });
    }

    pDevice_->get().waitIdle();

    pSwapchain_ = std::make_unique<Swapchain>(
            *pPhysicalDevice_,
            *pDevice_,
            *pSurface_,
            window.getExtent(),
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc);
    pSwapchain_->createDepthBuffer(*pDevice_);
    pSwapchain_->createFrameBuffers(*pDevice_, renderPass);
}

const vk::raii::Instance &GraphicContext::get() const noexcept {
    return vkInstance_;
}

const Device &GraphicContext::getDevice() const noexcept {
    return *pDevice_;
}

const PhysicalDevice &GraphicContext::getPhysicalDevice() const noexcept {
    return *pPhysicalDevice_;
}

const Surface &GraphicContext::getSurface() const noexcept {
    return *pSurface_;
}

const Swapchain &GraphicContext::getSwapchain() const noexcept {
    return *pSwapchain_;
}

#if defined(NDEBUG)
void GraphicContext::gatherLayers(const std::vector<std::string> &layers) {
#else
void GraphicContext::gatherLayers(const std::vector<std::string> &layers,
                                  const std::vector<vk::LayerProperties> &layerProperties) {
#endif
    enabledLayers_.reserve(layers.size());

    for (const auto &layer: layers) {
        assert(std::any_of(
                layerProperties.begin(),
                layerProperties.end(),
                [&layer](const vk::LayerProperties &layerProperty) { return layer == layerProperty.layerName; }));
        enabledLayers_.emplace_back(layer.c_str());
    }

#if !defined(NDEBUG)
    if (std::find(enabledLayers_.begin(), enabledLayers_.end(), "VK_LAYER_KHRONOS_validation") ==
                enabledLayers_.end() &&
        std::any_of(layerProperties.begin(), layerProperties.end(), [](const vk::LayerProperties &layerProperty) {
            return strcmp(layerProperty.layerName, "VK_LAYER_KHRONOS_validation") == 0;
        })) {
        enabledLayers_.emplace_back("VK_LAYER_KHRONOS_validation");
    }
    // TODO: log warning if validation layers are not available
#endif
}
#if defined(NDEBUG)
void GraphicContext::gatherExtensions(const std::vector<std::string> &extensions) {
#else
void GraphicContext::gatherExtensions(const std::vector<std::string> &extensions,
                                      const std::vector<vk::ExtensionProperties> &extensionProperties) {
#endif
    enabledExtensions_.reserve(extensions.size());

    for (const auto &extension: extensions) {
        assert(std::any_of(extensionProperties.begin(),
                           extensionProperties.end(),
                           [&extension](const vk::ExtensionProperties &extensionProperty) {
                               return extension == extensionProperty.extensionName;
                           }));
        enabledExtensions_.emplace_back(extension.c_str());
    }

#if !defined(NDEBUG)
    if (std::find(enabledExtensions_.begin(), enabledExtensions_.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) ==
                enabledExtensions_.end() &&
        std::any_of(extensionProperties.begin(),
                    extensionProperties.end(),
                    [](const vk::ExtensionProperties &extensionProperty) {
                        return strcmp(extensionProperty.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
                    })) {
        enabledExtensions_.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    // TODO: log warning if debug utils extension layers are not available
#endif
}

std::vector<std::string> GraphicContext::getGlfwRequiredExtensions() {
    uint32_t glfwExtensionsCount = 0;
    const char **glfwExtensionName = glfwWrapper(
            [&glfwExtensionsCount]() { return glfwGetRequiredInstanceExtensions(&glfwExtensionsCount); });
    return {glfwExtensionName, glfwExtensionName + glfwExtensionsCount};
}

std::vector<std::string> GraphicContext::getSurfacePlatformExtensions() {
    std::vector<std::string> extensions;
    extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    extensions.emplace_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    extensions.emplace_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    extensions.emplace_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MIR_KHR)
    extensions.emplace_back(VK_KHR_MIR_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_VI_NN)
    extensions.emplace_back(VK_NN_VI_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    extensions.emplace_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    extensions.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    extensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    extensions.emplace_back(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME);
#endif
    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                            void * /* pUserData */) {
    std::cerr << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
              << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType)) << ":\n";
    std::cerr << "\t"
              << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    std::cerr << "\t"
              << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    std::cerr << "\t"
              << "message         = <" << pCallbackData->pMessage << ">\n";

    return VK_FALSE;
}

static vk::DebugUtilsMessengerCreateInfoEXT makeDebugUtilsMessengerCreateInfoEXT() {
    // vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                       vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                       vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    return {vk::DebugUtilsMessengerCreateFlagsEXT{}, severityFlags, messageTypeFlags, &debugUtilsMessengerCallback};
}

} // namespace nae
