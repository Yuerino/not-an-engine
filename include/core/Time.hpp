#pragma once

#include <chrono>

namespace nae {

class Time {
public:
    explicit Time(double time = 0.0f) noexcept : time_(time) {}

    explicit operator float() const noexcept { return static_cast<float>(time_); }

    [[nodiscard]] double getSeconds() const noexcept { return time_; }

    [[nodiscard]] double getMilliseconds() const noexcept { return time_ * 1000.0f; }

    static Time now() {
        using namespace std::chrono;
        return Time(duration_cast<duration<double>>(high_resolution_clock::now().time_since_epoch()).count());
    }

    friend Time operator-(const Time &lhs, const Time &rhs) noexcept;

private:
    double time_;
};

inline Time operator-(const Time &lhs, const Time &rhs) noexcept {
    return Time{lhs.getSeconds() - rhs.getSeconds()};
}

} // namespace nae
