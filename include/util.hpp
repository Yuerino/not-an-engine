#pragma once

#include <cassert>
#include <fstream>

namespace nae::util {

inline std::string fragShaderPath;
inline std::string vertShaderPath;

template<typename TargetType, typename SourceType>
[[nodiscard]] inline TargetType checked_cast(SourceType value) {
    static_assert(sizeof(TargetType) <= sizeof(SourceType), "No need to cast from smaller to larger type!");
    static_assert(std::numeric_limits<SourceType>::is_integer, "Only integer types supported!");
    static_assert(!std::numeric_limits<SourceType>::is_signed, "Only unsigned types supported!");
    static_assert(std::numeric_limits<TargetType>::is_integer, "Only integer types supported!");
    static_assert(!std::numeric_limits<TargetType>::is_signed, "Only unsigned types supported!");
    assert(std::in_range<TargetType>(value));
    return static_cast<TargetType>(value);
}

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
