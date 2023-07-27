#pragma once

#include "gradient/color.hpp"

namespace ItG {

    /// @brief Linear interpolation between two colors
    template <IsColor TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        TColor result{};
        for (size_t i = 0; i < ColorSize<TColor>; i++) {
            result[i] = std::lerp(a[i], b[i], u);
        }
        return result;
    }

    /// @brief Linear interpolation between two colors
    template <IsColorN<1> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u)
            } };
    }

    /// @brief Linear interpolation between two colors
    template <IsColorN<2> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u)
            } };
    }

    /// @brief Linear interpolation between two colors
    template <IsColorN<3> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u)
            } };
    }

    /// @brief Linear interpolation between two colors
    template <IsColorN<4> TColor>
    inline [[nodiscard]] TColor lerp(const TColor& a, const TColor& b, const float& u) {
        return { { 
                std::lerp(a[0], b[0], u),
                std::lerp(a[1], b[1], u),
                std::lerp(a[2], b[2], u),
                std::lerp(a[3], b[3], u)
            } };
    }

    /// @brief Linear interpolation between two colors
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
