#ifndef NOT_AN_ENGINE_UTIL_HPP
#define NOT_AN_ENGINE_UTIL_HPP

namespace nae::util {

template<typename T>
inline constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
    return v < lo ? lo : hi < v ? hi : v;
}

} // namespace nae::util

#endif // NOT_AN_ENGINE_UTIL_HPP
