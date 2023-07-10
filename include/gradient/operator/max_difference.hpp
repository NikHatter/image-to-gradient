#pragma once

#include "gradient/linear.hpp"

namespace ItG::Gradient::Operator {

    template<size_t Size>
    struct MaxDifference {
        inline float operator()(const LinearIterator<Size>& first, const LinearIterator<Size>& last, const LinearIterator<Size>& a, const float& u) const {
            return operator()(lerp(first->color, last->color, u), a->color);
        }

        inline float operator()(const Color<Size>& a, const Color<Size>& b) const {
            Color<Size> diff = abs_diff(a, b);
            return *std::max_element(diff.begin(), diff.end());
        }
    };

}
