#pragma once

#include <algorithm>
#include <ranges>

#include "gradient/linear.hpp"

namespace ItG::Gradient::Strategy {

    /// @brief Find key with biggest difference to linear interpolation between range's ends.
    /// @tparam Range Linear gradient range
    template<LinearRange Range>
    struct FindFarthest {

        /// @brief Biggest difference to linear interpolation between range's ends
        /// @param range Linear gradient section
        /// @param distance_op distance operator
        /// @return Pair of iterator pointing to key with biggest distance and the distance value.
        /// Points to first element and has distance of -1 if key can't be determined or range is too small.
        [[nodiscard]] std::pair<LinearRange_Iterator<Range>, float > operator()(Range range, auto&& distance_op) const {
            using Iterator = LinearRange_Iterator<Range>;

            if (std::size(range) < 2) {
                return { std::begin(range), -1.f };
            }

            auto first = std::begin(range), last = std::prev( std::end(range) );

            // Key position to relative position in range.
            const float first_pos = first->position;
            const float last_pos = last->position;
            const float scale = 1.f / (last_pos - first_pos);
            
            auto projection = [&](const LinearRange_Value<Range>& key) { return distance_op(key, range, (key.position - first_pos) * scale); };
            Iterator fartherst = std::ranges::max_element(range, {}, projection);
            
            if ( fartherst == last || fartherst == first) {
                return { first, -1.f };
            }
            
            return { fartherst, projection(*fartherst) };
        }

    };
}
