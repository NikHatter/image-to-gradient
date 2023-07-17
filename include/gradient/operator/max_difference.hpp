#pragma once

#include "gradient/linear.hpp"

namespace ItG::Gradient::Operator {

    struct MaxDifference {
        template<LinearRange Range>
        inline [[nodiscard]] LinearRange_Color<Range> expected(Range range, float&& position) const {
            return lerp(range.front().color, range.back().color, std::forward<float>(position));
        }

        template<LinearRange Range>
        inline float operator()(const LinearRange_Value<Range>& value, Range range, float&& position) const {
            return operator()(
                value.color,
                expected(range, std::forward<float>(position))
            );
        }

        template<LinearRange Range>
        inline float operator()(const LinearRange_Iterator<Range>& value, Range range, float&& position) const {
            return operator()(
                value->color,
                expected(range, std::forward<float>(position))
            );
        }

        template<IsColor T>
        inline [[nodiscard]] float operator()(const T& a, const T& b) const {
            return std::ranges::max( abs_diff(a, b) );
        }

    };

}
