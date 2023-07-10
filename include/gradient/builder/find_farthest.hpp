#pragma once

#include "gradient/linear.hpp"

namespace ItG::Gradient::Stops {

    template<size_t Size, typename DistanceOp >
    struct FindFarthest {
        FindFarthest() = default;
        FindFarthest(const DistanceOp& op) : distance(op) {};

        std::pair<LinearIterator<Size>, float> operator()(LinearIterator<Size> first, LinearIterator<Size> last) const {
            LinearIterator<Size> fartherst = std::next(first);
            if (fartherst == last) {
                return { last, 0.f };
            }

            const float scale = 1.f / (last->position - first->position);
            float max_distance = distance(first, last, fartherst, (fartherst->position - first->position) * scale);
            for (LinearIterator<Size> color = fartherst + 1; color != last; ++color) {
                float cur_dist = distance(first, last, color, (color->position - first->position) * scale);
                if (cur_dist > max_distance) {
                    max_distance = cur_dist;
                    fartherst = color;
                }
            }

            return { fartherst, max_distance };
        }

    private:
        DistanceOp distance;
    };

}
