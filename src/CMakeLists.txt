set(SOURCE_FILES
    core/App.cpp
    core/Device.cpp
    core/GlfwApi.cpp
    core/GraphicContext.cpp
    core/PhysicalDevice.cpp
    core/Surface.cpp
    core/Swapchain.cpp
    core/Window.cpp

    renderer/Buffer.cpp
    renderer/CommandBuffer.cpp
    renderer/Descriptor.cpp
    renderer/Image.cpp
    renderer/Pipeline.cpp
    renderer/Renderer.cpp
    renderer/RenderPass.cpp
    renderer/Shader.cpp

    scene/Camera.cpp
    scene/Entity.cpp
    scene/EntityManager.cpp
    scene/Mesh.cpp
    scene/Model.cpp
    scene/SolidColorMaterial.cpp
    scene/Texture.cpp
    scene/TextureMaterial.cpp
    scene/Transform.cpp

    sample/main.cpp
    sample/BasicScene.cpp
    sample/FreeCamera.cpp
    sample/FreeCameraController.cpp)

add_executable(not-an-engine ${SOURCE_FILES})

set_target_properties(not-an-engine PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")

add_dependencies(not-an-engine nae-shaders)

#target_compile_options(not-an-engine PRIVATE
#    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

#target_link_options(not-an-engine PRIVATE
#    -fsanitize=address)
#
#target_compile_options(not-an-engine PRIVATE
#    -fsanitize=address)

target_compile_definitions(not-an-engine
    PUBLIC
    GLFW_INCLUDE_VULKAN
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_include_directories(not-an-engine
    PRIVATE
    ${GLM_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/dependency/tinyobjloader
    ${PROJECT_SOURCE_DIR}/dependency/stb
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(not-an-engine
    PRIVATE
    glfw
    PUBLIC
    Vulkan::Vulkan)
