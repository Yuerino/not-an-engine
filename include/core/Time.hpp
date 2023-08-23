#pragma once

#include <chrono>

namespace nae {

class Time {
public:
    explicit Time(float time = 0.0f) noexcept : time_(time) {}

    explicit operator float() const noexcept { return time_; }

    [[nodiscard]] float getSeconds() const noexcept { return time_; }

    [[nodiscard]] float getMilliseconds() const noexcept { return time_ * 1000.0f; }

    static Time now() {
        using namespace std::chrono;
        return Time(duration_cast<seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    }

private:
    float time_;
};

} // namespace nae
