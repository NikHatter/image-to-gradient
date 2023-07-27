#pragma once

#include <array>
#include <cmath>
#include <algorithm>
#include <type_traits>

namespace ItG {

    /// @brief Default color types
    namespace Color {
        /// @brief Grayscale color
        using Gray = float;
        /// @brief Grayscale color with alpha channel
        using GrayA = std::array<float, 2>;
        /// @brief RGB color
        using RGB = std::array<float, 3>;
        /// @brief RGB color with alpha channel
        using RGBA = std::array<float, 4>;
        /// @brief CMYK color
        using CMYK = std::array<float, 4>;
        /// @brief CMYK color with alpha channel
        using CMYKA = std::array<float, 5>;
    };

    /// @brief Type is arithmetic
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    /// @brief Type supports indexed access and operations required for gradient algorithms
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

    /// @brief Type can be used as color in gradient algorithms
    template<typename T>
    concept IsColor = Arithmetic<T> || IsColorArray<T>;

    // consteval instead of simple variable or struct due to disjuntion specification errors
    template<Arithmetic T>
    consteval size_t getSize() { return 1; };

    // consteval instead of simple variable or struct due to disjuntion specification errors
    template<IsColorArray T>
    consteval size_t getSize() { return std::size(T{}); };

    /// @brief Number of channels of a color
    template<typename T>
    constexpr size_t ColorSize = getSize<T>();

    /// @brief Color with N channels
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
}
