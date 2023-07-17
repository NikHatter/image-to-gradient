#pragma once

#include <array>
#include <cmath>
#include <algorithm>
#include <type_traits>

namespace ItG {

    namespace Color {
        using Gray = float;
        using GrayA = std::array<float, 2>;
        using RGB = std::array<float, 3>;
        using RGBA = std::array<float, 4>;
        using CMYK = std::array<float, 4>;
        using CMYKA = std::array<float, 5>;
    };

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    concept IsColorArray = requires (const T& x) { 
        { std::size(x) };
        { x[0] - x[0] };
        { x[0] + x[0] };
        { x[0] * x[0] };
        { x[0] * 0.5f };
        { std::abs(x[0]) };
        { std::lerp(x[0], x[0], 0.5f) };
    };

    template<typename T>
    concept IsColor = Arithmetic<T> || IsColorArray<T>;

    // consteval instead of simple variable or struct due to disjuntion specification errors
    template<Arithmetic T>
    consteval size_t getSize() { return 1; };

    template<IsColorArray T>
    consteval size_t getSize() { return std::size(T{}); };

    template<typename T>
    constexpr size_t ColorSize = getSize<T>();

    template<typename T, size_t N>
    concept IsColorN = IsColor<T> && requires { requires (ColorSize<T> == N); };

    static_assert(IsColor<Color::Gray>,  "Color::Gray is not a color type!");
    static_assert(IsColor<Color::GrayA>, "Color::GrayA is not a color type!");
    static_assert(IsColor<Color::RGB>,   "Color::RGB is not a color type!");
    static_assert(IsColor<Color::RGBA>,  "Color::RGBA is not a color type!");
    static_assert(IsColor<Color::CMYK>,  "Color::CMYK is not a color type!");
    static_assert(IsColor<Color::CMYKA>, "Color::CMYKA is not a color type!");

    static_assert(IsColorN<Color::Gray, 1>,  "Color::Gray size is not 1!");
    static_assert(IsColorN<Color::GrayA, 2>, "Color::GrayA size is not 2!");
    static_assert(IsColorN<Color::RGB, 3>,   "Color::RGB size is not 3!");
    static_assert(IsColorN<Color::RGBA, 4>,  "Color::RGBA size is not 4!");
    static_assert(IsColorN<Color::CMYK, 4>,  "Color::CMYK size is not 4!");
    static_assert(IsColorN<Color::CMYKA, 5>, "Color::CMYKA size is not 5!");


    template <IsColor TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        TColor result{};
        for (size_t i = 0; i < ColorSize<TColor>; i++) {
            result[i] = a[i] - b[i];
        }
        return result;
    }

    template <IsColorN<1> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0])
            } };
    }

    template <IsColorN<2> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1])
            } };
    }

    template <IsColorN<3> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2])
            } };
    }

    template <IsColorN<4> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2]),
                std::abs(a[3] - b[3])
            } };
    }

    template <IsColorN<5> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2]),
                std::abs(a[3] - b[3]),
                std::abs(a[4] - b[4])
            } };
    }


    template <IsColor TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        TColor result{};
        for (size_t i = 0; i < ColorSize<TColor>; i++) {
            result[i] = std::lerp(a[i], b[i], u);
        }
        return result;
    }

    template <IsColorN<1> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u)
            } };
    }

    template <IsColorN<2> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u)
            } };
    }

    template <IsColorN<3> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u)
            } };
    }

    template <IsColorN<4> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u),
                std::lerp(a[3], b[3], u)
            } };
    }

    template <IsColorN<5> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u),
                std::lerp(a[3], b[3], u),
                std::lerp(a[4], b[4], u)
            } };
    }
}
