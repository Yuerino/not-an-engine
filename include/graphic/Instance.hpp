#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace nae::graphic {

class Instance {
public:
    explicit Instance(const std::vector<std::string> &layers = {},
                      const std::vector<std::string> &extensions = {},
                      uint32_t apiVersion = VK_API_VERSION_1_3);

    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;

    [[nodiscard]] const vk::raii::Instance &operator*() const noexcept;

    /*!
     * @brief Get the list of required extensions for GLFW.
     *
     * @note GLFW should return the required surface extensions for the platform. If not consider using
     * getSurfacePlatformExtensions().
     */
    [[nodiscard]] static std::vector<std::string> getGlfwRequiredExtensions();

    /*!
     * @brief Get the required extensions of the platform for the surface.
     *
     * @note Consider using getGlfwRequiredExtensions() if possible.
     */
    [[nodiscard]] static std::vector<std::string> getSurfacePlatformExtensions();

private:
#if defined(NDEBUG)
    /*!
     * @brief Gather list of enabled layers for Vulkan.
     */
    void gatherLayers(const std::vector<std::string> &layers);

    /*!
     * @brief Gather list of enabled extensions for Vulkan.
     */
    void gatherExtensions(const std::vector<std::string> &extensions);
#else
    /*!
     * @brief Gather list of enabled layers for Vulkan.
     *
     * In debug mode, this function will also validate that all layers are available on the system and add the
     * validation layers if it is not present.
     */
    void gatherLayers(const std::vector<std::string> &layers, const std::vector<vk::LayerProperties> &layerProperties);

    /*!
     * @brief Gather list of enabled extensions for Vulkan.
     *
     * In debug mode, this function will also validate that all the extensions are available on the system and add the
     * debug utils messenger extension if it is not present.
     */
    void gatherExtensions(const std::vector<std::string> &extensions,
                          const std::vector<vk::ExtensionProperties> &extensionProperties);
#endif

    vk::raii::Context vkContext_;
    vk::raii::Instance vkInstance_{nullptr};
    std::vector<const char *> enabledLayers_;
    std::vector<const char *> enabledExtensions_;

#if !defined(NDEBUG)
    vk::raii::DebugUtilsMessengerEXT vkDebugUtilsMessenger_{nullptr};
#endif
};

} // namespace nae::graphic
