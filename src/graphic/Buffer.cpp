#include "graphic/Buffer.hpp"

namespace nae::graphic {

BufferData::BufferData(const Device &device,
                       vk::DeviceSize size,
                       vk::BufferUsageFlags usage,
                       vk::MemoryPropertyFlags propertyFlags)
    : buffer_{device.get(), {vk::BufferCreateFlags(), size, usage}},
      deviceMemory_{device.createDeviceMemory(buffer_.getMemoryRequirements(), propertyFlags)}
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
