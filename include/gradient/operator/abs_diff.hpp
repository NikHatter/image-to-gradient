#pragma once

#include "gradient/color.hpp"

namespace ItG::Gradient::Operator {

    /// @brief Magnitude of maximal difference between channel values
    /// @param a First color
    /// @param b Second color
    /// @return Magnitude of maximal difference between channel values
    template <IsColor TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        TColor result{};
        for (size_t i = 0; i < ColorSize<TColor>; i++) {
            result[i] = a[i] - b[i];
        }
        return result;
    }

    /// @brief Magnitude of maximal difference between channel values
    template <IsColorN<1> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0])
            } };
    }

    /// @brief Magnitude of maximal difference between channel values
    template <IsColorN<2> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1])
            } };
    }

    /// @brief Magnitude of maximal difference between channel values
    template <IsColorN<3> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2])
            } };
    }

    /// @brief Magnitude of maximal difference between channel values
    template <IsColorN<4> TColor>
    inline [[nodiscard]] TColor abs_diff(const TColor& a, const TColor& b) {
        return { {
                std::abs(a[0] - b[0]),
                std::abs(a[1] - b[1]),
                std::abs(a[2] - b[2]),
                std::abs(a[3] - b[3])
            } };
    }


    /// @brief Magnitude of maximal difference between channel values
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

}
