#pragma once

#include <array>
#include <cmath>
#include <algorithm>

namespace ItG {

    template<size_t Size>
    using Color = std::array<float, Size>;

    template <size_t Size>
    inline Color<Size> abs_diff(const Color<Size>& a, const Color<Size>& b) {
        Color<Size> result{};
        for (size_t i = 0; i < Size; i++) {
            result[i] = a[i] - b[i];
        }
        return result;
    }

    template <>
    inline Color<1> abs_diff(const Color<1>& a, const Color<1>& b) {
        return { {
                std::abs(a[0] - b[0])
            } };
    }

    template <>
    inline Color<2> abs_diff(const Color<2>& a, const Color<2>& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1])
            } };
    }

    template <>
    inline Color<3> abs_diff(const Color<3>& a, const Color<3>& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2])
            } };
    }

    template <>
    inline Color<4> abs_diff(const Color<4>& a, const Color<4>& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2]),
                std::abs(a[3] - b[3])
            } };
    }

    template <>
    inline Color<5> abs_diff(const Color<5>& a, const Color<5>& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2]),
                std::abs(a[3] - b[3]),
                std::abs(a[4] - b[4])
            } };
    }


    template <size_t Size>
    inline Color<Size> lerp(const Color<Size>& a, const Color<Size>& b, const float& u) {
        Color<Size> result{};
        for (size_t i = 0; i < Size; i++) {
            result[i] = std::lerp(a[i], b[i], u);
        }
        return result;
    }

    template <>
    inline Color<1> lerp(const Color<1>& a, const Color<1>& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u)
            } };
    }

    template <>
    inline Color<2> lerp(const Color<2>& a, const Color<2>& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u)
            } };
    }

    template <>
    inline Color<3> lerp(const Color<3>& a, const Color<3>& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u)
            } };
    }

    template <>
    inline Color<4> lerp(const Color<4>& a, const Color<4>& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u),
                std::lerp(a[3], b[3], u)
            } };
    }

    template <>
    inline Color<5> lerp(const Color<5>& a, const Color<5>& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u),
                std::lerp(a[3], b[3], u),
                std::lerp(a[4], b[4], u)
            } };
    }
}
