#pragma once

#include <map>
#include <stack>

#include "gradient/linear.hpp"
#include "gradient/builder/find_farthest.hpp"

namespace ItG::Gradient::Stops {

    template<size_t Size, typename DistanceOp >
    struct Approximate {
        Approximate() = default;
        Approximate(const DistanceOp& op) : find_farthest(op) {};

        float tolerance = 4.f / 255.f;

        void operator()(LinearIterator<Size> first, LinearIterator<Size> last, std::list<LinearIterator<Size>>& splits) const {
            using Interval = std::pair< LinearIterator<Size>, LinearIterator<Size> >;
            using IntervalStack = std::stack< Interval >;

            IntervalStack pending;
            pending.emplace(first, last);

            std::map<ptrdiff_t, LinearIterator<Size>> sorted_splits;

            while (!pending.empty()) {
                Interval current = pending.top();
                pending.pop();

                auto [fartherst, distance] = find_farthest(current.first, current.second);
                if (distance < tolerance || fartherst == current.second)
                    continue;

                pending.emplace(fartherst, current.second);
                pending.emplace(current.first, fartherst);

                sorted_splits.emplace(std::distance(first, fartherst), fartherst);
            }

            for (auto& sorted_split : sorted_splits) {
                splits.push_back(sorted_split.second);
            }
        }

    private:
        FindFarthest<Size, DistanceOp> find_farthest;
    };

    template<size_t Size, typename DistanceOp >
    struct ApproximateRecurse {
        ApproximateRecurse() = default;
        ApproximateRecurse(const DistanceOp& op) : find_farthest(op) {};

        float tolerance = 4.f / 255.f;

        void operator()(LinearIterator<Size> first, LinearIterator<Size> last, std::list<LinearIterator<Size>>& splits) const {
            auto [fartherst, distance] = find_farthest(first, last);
            if (distance < tolerance || fartherst == last)
                return;

            operator()(first, fartherst, splits);

            splits.push_back(fartherst);

            operator()(fartherst, last, splits);
        }

    private:
        FindFarthest<Size, DistanceOp> find_farthest;
    };

}
