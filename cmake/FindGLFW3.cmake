# Locate the glfw3 library

set(_glfw3_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${CMAKE_SOURCE_DIR}/dependency/glfw/includes"
    "C:/Program Files (x86)/glfw/include")

set(_glfw3_LIB_SEARCH_DIRS
    "/usr/lib"
    "/usr/local/lib"
    "${CMAKE_SOURCE_DIR}/dependency/glfw/lib"
    "C:/Program Files (x86)/glfw/lib-mingw-w64")

# Search for the header
FIND_PATH(GLFW3_INCLUDE_DIR "GLFW/glfw3.h"
    PATHS ${_glfw3_HEADER_SEARCH_DIRS})

# Search for the library
FIND_LIBRARY(GLFW3_LIBRARY NAMES glfw3 glfw
    PATHS ${_glfw3_LIB_SEARCH_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(glfw3 DEFAULT_MSG
    GLFW3_LIBRARY GLFW3_INCLUDE_DIR)
