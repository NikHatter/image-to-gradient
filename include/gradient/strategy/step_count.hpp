#pragma once

#include <map>
#include <list>
#include <span>

#include "gradient/linear.hpp"
#include "gradient/strategy/find_farthest.hpp"
#include "gradient/operator/max_difference.hpp"

namespace ItG::Gradient::Strategy {

    // TODO check view support

    /// @brief Extract exact number of keys (not including gradient start/end).
    /// The keys are extracted in order of most significance.
    struct ColorCount {

        /// @brief Target number of keys.
        size_t count = 4;

        /// @brief Extract keys from original range.
        /// @param original Original gradient data (full gradient or sub-section)
        /// @param extracted Output gradient data (extracted values are appended at end)
        /// @param distance_op Operator for calculating distance
        template<LinearRange Range >
        void operator()(Range range, LinearData auto& splits, auto&& distance_op) const {
            using namespace std;

            using Iterator = LinearRange_Iterator<Range>;
            using IntervalEnds = std::list< Iterator >;

            IntervalEnds current_intervals;
            current_intervals.push_front(begin(range));
            current_intervals.push_back(prev(end(range))); // TODO prev?

            map<ptrdiff_t, Iterator> sorted_splits;
            FindFarthest<Range> find_farthest;

            for (size_t i = 0; i < count; i++) {
                auto interval_first = begin(current_intervals);
                auto interval_second = next(interval_first);

                map<float, Iterator> potential_splits;
                for (auto last = end(current_intervals); interval_second != last; advance(interval_first, 1), advance(interval_second, 1) ) {
                    auto [fartherst, distance] = find_farthest(ranges::subrange{ *interval_first, next(*interval_second) }, forward<decltype(distance_op)>(distance_op));
                    if (fartherst != *interval_second) {
                        potential_splits[distance] = fartherst;
                    }
                }

                if (empty(potential_splits))
                    break;

                Iterator fartherst = prev( end(potential_splits) )->second;
                sorted_splits.emplace( distance(begin(range), fartherst), fartherst );

                auto place = begin(current_intervals);
                for (auto last = end(current_intervals); place != last; advance(place, 1)) {
                    if (distance(*place, fartherst) < 1)
                        break;
                }
                current_intervals.insert(place, fartherst);
            }

            for (auto& split : views::values(sorted_splits)) {
                splits.emplace_back(*split);
            }
        }

    };

    /// @brief Perform specefied number of key extractions.
    /// Each time keys with comparable difference to current output gradient are selected.
    struct StepCount {

        /// @brief Number of extraction steps.
        size_t count = 4;
        /// @brief Relative difference similarity factor. In range [0, 1].
        /// Each extraction step adds keys with distance to gradient no lower than max distance in step * (1 - stop_distance)
        float stop_distance = 0.2f;

        /// @brief Extract keys from original range.
        /// @param original Original gradient data (full gradient or sub-section)
        /// @param extracted Output gradient data (extracted values are appended at end)
        /// @param distance_op Operator for calculating distance
        template<LinearRange Range >
        void operator()(Range range, LinearData auto& splits, auto&& distance_op) const {
            using namespace std;

            using Iterator = LinearRange_Iterator<Range>;
            using IntervalEnds = std::list< Iterator >;

            IntervalEnds current_intervals;
            current_intervals.push_front(begin(range));
            current_intervals.push_back(prev(end(range))); // TODO prev?

            map<ptrdiff_t, Iterator> sorted_splits;
            FindFarthest<Range> find_farthest;

            for (size_t i = 0; i < count; i++) {
                auto interval_first = begin(current_intervals);
                auto interval_second = next(interval_first);

                multimap<float, Iterator> potential_splits;
                for (auto last = end(current_intervals); interval_second != last; advance(interval_first, 1), advance(interval_second, 1)) {
                    auto [fartherst, distance] = find_farthest(ranges::subrange{ *interval_first, next(*interval_second) }, forward<decltype(distance_op)>(distance_op));
                    if (fartherst != *interval_second) {
                        potential_splits.emplace(distance, fartherst);
                    }
                }

                if (empty(potential_splits))
                    break;

                for (auto split = potential_splits.lower_bound(prev(end(potential_splits))->first * (1.f - clamp(stop_distance, 0.f, 1.f))), last = end(potential_splits) ; split != last; advance(split, 1)) {

                    Iterator fartherst = split->second;
                    sorted_splits.emplace(distance(begin(range), fartherst), fartherst);

                    // TODO: lower_bound by std::distance
                    auto place = current_intervals.begin();
                    for (auto last = end(current_intervals); place != last; advance(place, 1)) {
                        if (distance(*place, fartherst) < 1)
                            break;
                    }
                    current_intervals.insert(place, fartherst);
                }
            }

            for (auto& split : views::values(sorted_splits)) {
                splits.emplace_back(*split);
            }
        }

    };
}
