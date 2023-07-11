#include "util/vulkan.hpp"

#include <iostream>

#include <GLFW/glfw3.h>

namespace nae::util::vk {

/*!
 * @brief Return list of enabled layers for Vulkan.
 *
 * In debug mode, this function will also validate that all layers are available on the system and add the validation
 * layers if it is not present.
 */
std::vector<const char *> gatherLayers(const std::vector<std::string> &layers
#ifndef NDEBUG
                                       ,
                                       const std::vector<::vk::LayerProperties> &layerProperties
#endif
) {
    std::vector<const char *> enabledLayers;
    enabledLayers.reserve(layers.size());

    for (const auto &layer: layers) {
        assert(std::find_if(layerProperties.begin(),
                            layerProperties.end(),
                            [&layer](const ::vk::LayerProperties &layerProperty) {
                                return layerProperty.layerName == layer;
                            }) != layerProperties.end());
        enabledLayers.push_back(layer.c_str());
    }

#ifndef NDEBUG
    if (std::find(enabledLayers.begin(), enabledLayers.end(), "VK_LAYER_KHRONOS_validation") == enabledLayers.end() &&
        std::find_if(layerProperties.begin(), layerProperties.end(), [](const ::vk::LayerProperties &layerProperty) {
            return strcmp(layerProperty.layerName, "VK_LAYER_KHRONOS_validation") == 0;
        }) != layerProperties.end()) {
        enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
    }
    // TODO: log warning if validation layers are not available
#endif

    return enabledLayers;
}

/*!
 * @brief Return list of enabled extensions for Vulkan.
 *
 * In debug mode, this function will also validate that all extensions are available on the system and add the debug
 * utils messenger extension if it is not present.
 */
std::vector<const char *> gatherExtensions(
#ifndef NDEBUG
        const std::vector<::vk::ExtensionProperties> &extensionProperties
#endif
) {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensionName = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> enabledExt;
    enabledExt.reserve(glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        assert(std::find_if(extensionProperties.begin(),
                            extensionProperties.end(),
                            [&glfwExtensionName, i](const ::vk::ExtensionProperties &extensionProperty) {
                                return strcmp(extensionProperty.extensionName, glfwExtensionName[i]) == 0;
                            }) != extensionProperties.end());
        enabledExt.push_back(glfwExtensionName[i]);
    }

#ifndef NDEBUG
    if (std::find(enabledExt.begin(), enabledExt.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == enabledExt.end() &&
        std::find_if(extensionProperties.begin(),
                     extensionProperties.end(),
                     [](const ::vk::ExtensionProperties &extensionProperty) {
                         return strcmp(extensionProperty.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
                     }) != extensionProperties.end()) {
        enabledExt.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    // TODO: log warning if debug utils extension layers are not available
#endif

    return enabledExt;
}

/*!
 * @brief Callback for debug utils messenger extension.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL
debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                            [[maybe_unused]] void *pUserData) {
    std::cerr << ::vk::to_string(static_cast<::vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
              << ::vk::to_string(static_cast<::vk::DebugUtilsMessageTypeFlagsEXT>(messageType)) << ":\n";
    std::cerr << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    std::cerr << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    std::cerr << std::string("\t") << "message         = <" << pCallbackData->pMessage << ">\n";

    return VK_FALSE;
}

::vk::DebugUtilsMessengerCreateInfoEXT makeDebugUtilsMessengerCreateInfoEXT() {
    ::vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(::vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                                          ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                          ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    ::vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                         ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                         ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    return {{}, severityFlags, messageTypeFlags, &debugUtilsMessengerCallback};
}

::vk::raii::Instance createInstance(::vk::raii::Context &vkContext,
                                    const std::string &appName,
                                    uint32_t appVersion,
                                    const std::string &engineName,
                                    uint32_t engineVersion,
                                    [[maybe_unused]] const std::vector<std::string> &layers,
                                    [[maybe_unused]] const std::vector<std::string> &extensions,
                                    uint32_t apiVersion) {
    ::vk::ApplicationInfo appInfo{appName.c_str(), appVersion, engineName.c_str(), engineVersion, apiVersion};

#ifdef NDEBUG
    std::vector<const char *> enabledLayers = gatherLayers(layers);
    std::vector<const char *> enabledExtensions = gatherExtensions();

    ::vk::StructureChain<::vk::InstanceCreateInfo> instanceCreateInfoChain{
            {{}, &appInfo, enabledLayers, enabledExtensions}};

#else

    std::vector<const char *> enabledLayers = gatherLayers(layers, vkContext.enumerateInstanceLayerProperties());
    std::vector<const char *> enabledExtensions = gatherExtensions(vkContext.enumerateInstanceExtensionProperties());

    ::vk::StructureChain<::vk::InstanceCreateInfo, ::vk::DebugUtilsMessengerCreateInfoEXT> instanceCreateInfoChain{
            {{}, &appInfo, enabledLayers, enabledExtensions},
            makeDebugUtilsMessengerCreateInfoEXT()};
#endif

    return ::vk::raii::Instance{vkContext, instanceCreateInfoChain.get<::vk::InstanceCreateInfo>()};
}

} // namespace nae::util::vk
