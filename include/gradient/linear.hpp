#pragma once

#include <vector>
#include <iterator>

#include "color.hpp"

namespace ItG::Gradient {

    template<size_t Size>
    struct Key {
        Color<Size> color;
        float position = 0;

        Key() {
            color.fill(0.f);
        }
        Key(const Color<Size>& color, const float& position) : color(color), position(position)
        {}
    };

    template<size_t Size>
    using Linear = std::vector< Key<Size> >;

    template<size_t Size>
    using LinearIterator = Linear<Size>::const_iterator;

}
