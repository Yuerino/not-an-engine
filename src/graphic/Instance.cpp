#include "graphic/Instance.hpp"

#include <cassert>
#include <iostream>

#include <GLFW/glfw3.h>

#include "GlfwApi.hpp"

namespace nae::graphic {

static vk::DebugUtilsMessengerCreateInfoEXT makeDebugUtilsMessengerCreateInfoEXT();

Instance::Instance(const std::vector<std::string> &layers,
                   const std::vector<std::string> &extensions,
                   uint32_t apiVersion)
    : vkContext_{} {
    vk::ApplicationInfo appInfo{"Application made with not-an-engine",
                                VK_MAKE_API_VERSION(0, 1, 0, 0),
                                "not-an-engine",
                                VK_MAKE_API_VERSION(0, 1, 0, 0),
                                apiVersion};

#if defined(NDEBUG)
    gatherLayers(layers);
    gatherExtensions(extensions);

    vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfoChain{
            {vk::InstanceCreateFlags{}, &appInfo, enabledLayers_, enabledExtensions_}};
#else
    gatherLayers(layers, vkContext_.enumerateInstanceLayerProperties());
    gatherExtensions(extensions, vkContext_.enumerateInstanceExtensionProperties());

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instanceCreateInfoChain{
            {vk::InstanceCreateFlags{}, &appInfo, enabledLayers_, enabledExtensions_},
            makeDebugUtilsMessengerCreateInfoEXT()};
#endif

    vkInstance_ = vk::raii::Instance{vkContext_, instanceCreateInfoChain.get<vk::InstanceCreateInfo>()};

#if !defined(NDEBUG)
    vkDebugUtilsMessenger_ = vk::raii::DebugUtilsMessengerEXT{vkInstance_, makeDebugUtilsMessengerCreateInfoEXT()};
#endif
}

const vk::raii::Instance &Instance::operator*() const noexcept {
    return vkInstance_;
}

#if defined(NDEBUG)
void Instance::gatherLayers(const std::vector<std::string> &layers) {
#else
void Instance::gatherLayers(const std::vector<std::string> &layers,
                            const std::vector<vk::LayerProperties> &layerProperties) {
#endif
    enabledLayers_.reserve(layers.size());

    for (const auto &layer: layers) {
        assert(std::any_of(
                layerProperties.begin(),
                layerProperties.end(),
                [&layer](const vk::LayerProperties &layerProperty) { return layerProperty.layerName == layer; }));
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
void Instance::gatherExtensions(const std::vector<std::string> &extensions) {
#else
void Instance::gatherExtensions(const std::vector<std::string> &extensions,
                                const std::vector<vk::ExtensionProperties> &extensionProperties) {
#endif
    enabledExtensions_.reserve(extensions.size());

    for (const auto &extension: extensions) {
        assert(std::any_of(extensionProperties.begin(),
                           extensionProperties.end(),
                           [&extension](const vk::ExtensionProperties &extensionProperty) {
                               return extensionProperty.extensionName == extension;
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

std::vector<std::string> Instance::getGlfwRequiredExtensions() {
    uint32_t glfwExtensionsCount = 0;
    const char **glfwExtensionName = glfwWrapper(
            [&glfwExtensionsCount]() { return glfwGetRequiredInstanceExtensions(&glfwExtensionsCount); });

    std::vector<std::string> glfwExtensions;
    glfwExtensions.reserve(glfwExtensionsCount);

    for (uint32_t i = 0; i < glfwExtensionsCount; ++i) {
        glfwExtensions.emplace_back(glfwExtensionName[i]);
    }

    return glfwExtensions;
}

std::vector<std::string> Instance::getSurfacePlatformExtensions() {
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

} // namespace nae::graphic
