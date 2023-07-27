#pragma once

#include <map>
#include <stack>
#include <span>
#include <ranges>

#include "gradient/linear.hpp"
#include "gradient/strategy/find_farthest.hpp"

namespace ItG::Gradient::Strategy {

    // TODO check view support

    /// @brief Extract keys that are close enough to original gradient. Non-recusive version.
    struct Approximate {
        /// @brief Maximal distance between extracted end original gradient.
        float tolerance = 4.f / 255.f;

        /// @brief Extract keys from original range.
        /// @param original Original gradient data (full gradient or sub-section)
        /// @param extracted Output gradient data (extracted values are appended at end)
        /// @param distance_op Operator for calculating distance
        template<LinearRange Range>
        void operator()(Range original, LinearData auto& extracted, auto&& distance_op) const {
            using namespace std;

            using Iterator = LinearRange_Iterator<Range>;
            using Span = LinearRange_Subrange<Range>;
            using SpanStack = std::stack< Span >;

            /// Stack with unprocessed sub-gradients
            SpanStack pending;
            pending.emplace(begin(original), end(original));

            // Extracted keys, by order in original array to support banding.
            // Used for correct ordering of keys with same position but different color)
            map<ptrdiff_t, Iterator> splits;

            FindFarthest<Range> find_farthest;

            while (!pending.empty()) {
                Span current = pending.top();
                pending.pop();

                auto [fartherst, distance] = find_farthest(current, forward<decltype(distance_op)>(distance_op));
                // Remove sub-gradient if it's close enough
                if (distance <= tolerance)
                    continue;

                // Add left and right sub-gradient so that left is processed first
                pending.emplace(fartherst, end(current));
                pending.emplace(begin(current), next(fartherst));

                splits.emplace(std::distance(begin(original), fartherst), fartherst);
            }

            /// Build extracted gradient
            for (auto& split : views::values(splits)) {
                extracted.emplace_back(*split);
            }
        }

    };

    /// @brief Extract keys that are close enough to original gradient. Recusive version.
    struct ApproximateRecurse {
        /// @brief Maximal distance between extracted end original gradient.
        float tolerance = 4.f / 255.f;

        /// @brief Extract keys from original range.
        /// @param original Original gradient data (full gradient or sub-section)
        /// @param extracted Output gradient data (extracted values are appended at end)
        /// @param distance_op Operator for calculating distance
        template<LinearRange Range>
        void operator()(Range range, LinearData auto& splits, auto&& distance_op) const {
            using namespace std;

            using Iterator = Range::iterator;
            FindFarthest<Range> find_farthest;

            auto [fartherst, distance] = find_farthest(range, forward<decltype(distance_op)>(distance_op));
            // Stop if sub-gradient is close enough
            if (distance <= tolerance)
                return;

            /// Build extracted gradient from sub-gradiet extraction result left-to-right

            operator()(ranges::subrange(begin(range), next(fartherst)), splits, forward<decltype(distance_op)>(distance_op));

            splits.emplace_back(*fartherst);

            operator()(ranges::subrange(fartherst, end(range)), splits, forward<decltype(distance_op)>(distance_op));
        }

    };

}
