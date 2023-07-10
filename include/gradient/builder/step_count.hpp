#pragma once

#include "gradient/linear.hpp"
#include "gradient/builder/find_farthest.hpp"
#include "gradient/operator/max_difference.hpp"

namespace ItG::Gradient::Stops {

    template<size_t Size, typename DistanceOp >
    struct ColorCount {
        ColorCount() = default;
        ColorCount(const DistanceOp& op) : find_farthest(op) {};

        size_t count = 4;

        void operator()(LinearIterator<Size> first, LinearIterator<Size> last, std::list<LinearIterator<Size>>& splits) const {
            using IntervalEnds = std::list< LinearIterator<Size> >;

            IntervalEnds current_intervals;
            current_intervals.push_front(first);
            current_intervals.push_back(last);

            std::map<ptrdiff_t, LinearIterator<Size>> sorted_splits;

            for (size_t i = 0; i < count; i++) {
                auto interval_first = current_intervals.begin();
                auto interval_second = std::next(interval_first);

                std::map<float, LinearIterator<Size>> potential_splits;
                while (interval_second != current_intervals.end()) {

                    auto [fartherst, distance] = find_farthest(*interval_first, *interval_second);
                    if (fartherst != *interval_second) {
                        potential_splits[distance] = fartherst;
                    }

                    std::advance(interval_first, 1);
                    std::advance(interval_second, 1);
                }

                if (potential_splits.empty())
                    break;

                LinearIterator<Size> fartherst = std::prev(potential_splits.end())->second;
                sorted_splits.emplace(std::distance(first, fartherst), fartherst);

                auto place = current_intervals.begin();
                while (place != current_intervals.end()) {
                    if (std::distance(*place, fartherst) < 0)
                        break;

                    std::advance(place, 1);
                }
                current_intervals.insert(place, fartherst);
            }

            for (auto& sorted_split : sorted_splits) {
                splits.push_back(sorted_split.second);
            }
        }

    private:
        FindFarthest<Size, DistanceOp> find_farthest;
    };

    template<size_t Size, typename DistanceOp >
    struct StepCount {
        StepCount() = default;
        StepCount(const DistanceOp& op) : find_farthest(op) {};

        size_t count = 4;
        float stop_distance = 0.2f;

        void operator()(LinearIterator<Size> first, LinearIterator<Size> last, std::list<LinearIterator<Size>>& splits) const {
            using IntervalEnds = std::list< LinearIterator<Size> >;

            IntervalEnds current_intervals;
            current_intervals.push_front(first);
            current_intervals.push_back(last);

            std::map<ptrdiff_t, LinearIterator<Size>> sorted_splits;

            for (size_t i = 0; i < count; i++) {
                auto interval_first = current_intervals.begin();
                auto interval_second = std::next(interval_first);

                std::multimap<float, LinearIterator<Size>> potential_splits;
                while (interval_second != current_intervals.end()) {

                    auto [fartherst, distance] = find_farthest(*interval_first, *interval_second);
                    if (fartherst != *interval_second) {
                        potential_splits.emplace(distance, fartherst);
                    }

                    std::advance(interval_first, 1);
                    std::advance(interval_second, 1);
                }

                if (potential_splits.empty())
                    break;

                for (auto split = potential_splits.lower_bound(std::prev(potential_splits.end())->first * (1.f - stop_distance)); split != potential_splits.end(); ++split) {

                    LinearIterator<Size> fartherst = split->second;
                    sorted_splits.emplace(std::distance(first, fartherst), fartherst);

                    // TODO: lower_bound by std::distance
                    auto place = current_intervals.begin();
                    while (place != current_intervals.end()) {
                        if (std::distance(*place, fartherst) < 0)
                            break;

                        std::advance(place, 1);
                    }
                    current_intervals.insert(place, fartherst);
                }
            }

            for (auto& sorted_split : sorted_splits) {
                splits.push_back(sorted_split.second);
            }
        }

    private:
        FindFarthest<Size, DistanceOp> find_farthest;
    };
}
