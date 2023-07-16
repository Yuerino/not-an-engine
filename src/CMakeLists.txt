set(SOURCE_FILES
    main.cpp
    App.cpp
    GlfwApi.cpp
    Window.cpp
    graphic/Graphic.cpp
    graphic/Instance.cpp
    graphic/PhysicalDevice.cpp
    graphic/Device.cpp
    graphic/Surface.cpp
    graphic/SwapChainData.cpp)

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

#target_compile_options(not-an-engine PRIVATE
#    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)

target_compile_definitions(not-an-engine
    PUBLIC
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE)

target_include_directories(not-an-engine
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(not-an-engine
    PUBLIC
    Vulkan::Vulkan
    PRIVATE
    glfw)
