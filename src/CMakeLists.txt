set(SOURCE_FILES
    main.cpp
    core/App.cpp
    core/Device.cpp
    core/GlfwApi.cpp
    core/GraphicInstance.cpp
    core/PhysicalDevice.cpp
    core/Scene.cpp
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
    renderer/Shader.cpp)

find_package(glfw3 3.3 CONFIG QUIET)
if (NOT glfw3_FOUND)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW Lib only")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW Lib only")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW Lib only")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW Lib only")
    set(GLFW_INSTALL OFF CACHE BOOL "GLFW Lib only")
    add_subdirectory(${PROJECT_SOURCE_DIR}/dependency/glfw glfw)
    set_target_properties(glfw PROPERTIES FOLDER "dependency")
endif ()

add_executable(not-an-engine ${SOURCE_FILES})

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
    ${PROJECT_SOURCE_DIR}/dependency/tinyobjloader
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(not-an-engine
    PRIVATE
    glfw
    PUBLIC
    Vulkan::Vulkan)
