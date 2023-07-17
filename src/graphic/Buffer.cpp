#include "graphic/Buffer.hpp"

#include "graphic/Device.hpp"

namespace nae::graphic {

BufferData::BufferData(const vk::raii::PhysicalDevice &physicalDevice,
                       const vk::raii::Device &device,
                       vk::DeviceSize size,
                       vk::BufferUsageFlags usage,
                       vk::MemoryPropertyFlags propertyFlags)
    : buffer_{device, {vk::BufferCreateFlags(), size, usage}},
      deviceMemory_{createDeviceMemory(device,
                                       physicalDevice.getMemoryProperties(),
                                       buffer_.getMemoryRequirements(),
                                       propertyFlags)}
#if !defined(NDEBUG)
      ,
      size_{size},
      usage_{usage},
      propertyFlags_{propertyFlags}
#endif
{
    buffer_.bindMemory(*deviceMemory_, 0);
}

} // namespace nae::graphic
