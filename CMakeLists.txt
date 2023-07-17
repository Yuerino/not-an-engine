cmake_minimum_required(VERSION 3.25)
project(not-an-engine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/CMake" CACHE STRING "Modules for CMake" FORCE)

find_package(Vulkan REQUIRED)

add_subdirectory(src)
add_subdirectory(shader)
