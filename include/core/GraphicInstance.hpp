#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "core/Device.hpp"
#include "core/PhysicalDevice.hpp"
#include "core/Surface.hpp"
#include "core/Swapchain.hpp"
#include "core/Window.hpp"
#include "renderer/RenderPass.hpp"

namespace nae {

class Window;
class Device;
class PhysicalDevice;
class Surface;
class Swapchain;

class GraphicInstance {
public:
    explicit GraphicInstance(const std::vector<std::string> &layers = {},
                             const std::vector<std::string> &extensions = {},
                             uint32_t apiVersion = VK_API_VERSION_1_3);
    ~GraphicInstance() = default;

    GraphicInstance(const GraphicInstance &) = delete;
    GraphicInstance &operator=(const GraphicInstance &) = delete;
    GraphicInstance(GraphicInstance &&) = default;
    GraphicInstance &operator=(GraphicInstance &&) = default;

    void initDeviceAndSwapchain(const Window &window);

    void createDepthAndFrameBuffers(const RenderPass &renderPass);

    void recreateSwapchain(const Window &window, const RenderPass &renderPass);

    [[nodiscard]] const vk::raii::Instance &get() const noexcept;
    [[nodiscard]] const Device &getDevice() const noexcept;
    [[nodiscard]] const PhysicalDevice &getPhysicalDevice() const noexcept;
    [[nodiscard]] const Surface &getSurface() const noexcept;
    [[nodiscard]] const Swapchain &getSwapchain() const noexcept;

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

    vk::raii::Context vkContext_{};
    vk::raii::Instance vkInstance_{nullptr};
    std::unique_ptr<Device> pDevice_;
    std::unique_ptr<PhysicalDevice> pPhysicalDevice_;
    std::unique_ptr<Surface> pSurface_;
    std::unique_ptr<Swapchain> pSwapchain_;

    std::vector<const char *> enabledLayers_;
    std::vector<const char *> enabledExtensions_;

#if !defined(NDEBUG)
    vk::raii::DebugUtilsMessengerEXT vkDebugUtilsMessenger_{nullptr};
#endif
};

} // namespace nae
