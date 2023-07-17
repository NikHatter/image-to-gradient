#pragma once

#include <map>
#include <stack>
#include <span>
#include <ranges>

#include "gradient/linear.hpp"
#include "gradient/strategy/find_farthest.hpp"

namespace ItG::Gradient::Strategy {

    // TODO view support

    struct Approximate {
        float tolerance = 4.f / 255.f;

        template<LinearRange Range>
        void operator()(Range original, LinearData auto& extracted, auto&& distance_op) const {
            using namespace std;

            using Iterator = LinearRange_Iterator<Range>;
            using Span = LinearRange_Subrange<Range>;
            using SpanStack = std::stack< Span >;

            SpanStack pending;
            pending.emplace(begin(original), end(original));

            map<ptrdiff_t, Iterator> splits;

            FindFarthest<Range> find_farthest;

            while (!pending.empty()) {
                Span current = pending.top();
                pending.pop();

                auto [fartherst, distance] = find_farthest(current, forward<decltype(distance_op)>(distance_op));
                if (distance <= tolerance)
                    continue;

                pending.emplace(fartherst, end(current));
                pending.emplace(begin(current), next(fartherst));

                splits.emplace(std::distance(begin(original), fartherst), fartherst);
            }

            for (auto& split : views::values(splits)) {
                extracted.emplace_back(*split);
            }
        }

    };

    struct ApproximateRecurse {

        float tolerance = 4.f / 255.f;

        template<LinearRange Range>
        void operator()(Range range, LinearData auto& splits, auto&& distance_op) const {
            using namespace std;

            using Iterator = Range::iterator;
            FindFarthest<Range> find_farthest;

            auto [fartherst, distance] = find_farthest(range, forward<decltype(distance_op)>(distance_op));
            if (distance <= tolerance)
                return;

            operator()(ranges::subrange(begin(range), next(fartherst)), splits, forward<decltype(distance_op)>(distance_op));

            splits.emplace_back(*fartherst);

            operator()(ranges::subrange(fartherst, end(range)), splits, forward<decltype(distance_op)>(distance_op));
        }

    };

}
