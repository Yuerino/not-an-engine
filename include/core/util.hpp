#pragma once

#include <fstream>

#include "renderer/Buffer.hpp"

namespace nae::util {

inline std::string fragShaderPath;
inline std::string vertShaderPath;


[[nodiscard]] inline std::vector<char> readFile(const std::string &path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    auto fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();
    return buffer;
}

} // namespace nae::util
