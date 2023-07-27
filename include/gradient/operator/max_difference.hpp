#pragma once

#include "gradient/operator/abs_diff.hpp"
#include "gradient/operator/lerp.hpp"
#include "gradient/linear.hpp"

namespace ItG::Gradient::Operator {

    /// @brief Channel difference magnitude operator.
    /// Calculates magnitude of maximal difference between channel values.
    struct MaxDifference {

        /// @brief Calculate color expected at posiion
        template<LinearRange Range>
        inline [[nodiscard]] LinearRange_Color<Range> expected(Range range, float&& position) const {
            return lerp(range.front().color, range.back().color, std::forward<float>(position));
        }

        /// @brief Difference with expected color
        template<LinearRange Range>
        inline float operator()(const LinearRange_Value<Range>& value, Range range, float&& position) const {
            return operator()(
                value.color,
                expected(range, std::forward<float>(position))
            );
        }

        /// @brief Difference with color expected at posiion
        template<LinearRange Range>
        inline float operator()(const LinearRange_Iterator<Range>& value, Range range, float&& position) const {
            return operator()(
                value->color,
                expected(range, std::forward<float>(position))
            );
        }

        /// @brief Magnitude of maximal difference between channel values
        template<IsColor T>
        inline [[nodiscard]] float operator()(const T& a, const T& b) const {
            return std::ranges::max( abs_diff(a, b) );
        }

    };

}
