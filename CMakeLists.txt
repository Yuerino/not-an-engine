cmake_minimum_required(VERSION 3.25)
project(not-an-engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/CMake" CACHE STRING "Modules for CMake" FORCE)

find_package(Vulkan REQUIRED)

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

find_package(glm CONFIG QUIET)
if (NOT glm_FOUND)
    add_subdirectory(${PROJECT_SOURCE_DIR}/dependency/glm glm)
    set_target_properties(glm PROPERTIES FOLDER "dependency")
endif ()

add_subdirectory(src)
add_subdirectory(shader)

file(COPY model DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shader DESTINATION ${CMAKE_BINARY_DIR})
