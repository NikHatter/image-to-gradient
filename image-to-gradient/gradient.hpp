#pragma once

#include <list>
#include <map>
#include <stack>
#include <vector>
#include <iterator>
#include <limits>

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

    template<size_t Size, typename DistanceOp = MaxDifference<Size> >
    struct LinearBuilder {
        float tolerance = 4.f / 255.f;
        std::array<float, 2> out_range{ { 0, 1} };

        Gradient::Linear<Size> from_linear(
            Gradient::Linear<Size>& gradient
        ) {
            if (gradient.empty())
                return {};

            std::list<LinearIterator<Size>> keys;
            keys.push_back(gradient.begin());
            split(gradient.begin(), gradient.end() - 1, keys);
            keys.push_back(gradient.end() - 1);

            return build(gradient.begin(), gradient.end(), keys);
        }

        Gradient::Linear<Size> from_colors(
            std::vector< Color<Size> >& colors
        ) {
            if (colors.empty())
                return {};

            Linear<Size> gradient;
            gradient.reserve(colors.size());
            std::transform(colors.begin(), colors.end(), std::back_inserter(gradient), [&](auto& v) {
                return Key<Size>{ v, gradient.size() / float(colors.size() - 1) };
            });
            return from_linear(gradient);
        }

    protected:
        DistanceOp distance;

        // returns iterator to key with highest difference with linear gradient between first and last (within tolerance); or last if no such key found
        LinearIterator<Size> find_farthest(LinearIterator<Size> first, LinearIterator<Size> last) {
            LinearIterator<Size> fartherst = std::next(first);
            if (fartherst == last) {
                return last;
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

            if (max_distance < tolerance) {
                return last;
            }

            return fartherst;
        }

        void split_recursion(LinearIterator<Size> first, LinearIterator<Size> last, std::list<LinearIterator<Size>>& splits, bool left = false) {
            LinearIterator<Size> fartherst = find_farthest(first, last);
            if (fartherst == last)
                return;

            split_recursion(first, fartherst, splits, true);

            splits.push_back(fartherst);

            split_recursion(fartherst, last, splits);
        }

        void split(LinearIterator<Size> first, LinearIterator<Size> last, std::list<LinearIterator<Size>>& splits) {
            using Interval = std::pair< LinearIterator<Size>, LinearIterator<Size> >;
            using IntervalStack = std::stack< Interval >;

            IntervalStack pending;
            pending.emplace(first, last);

            std::map<ptrdiff_t, LinearIterator<Size>> sorted_splits;

            while (!pending.empty()) {
                Interval current = pending.top();
                pending.pop();

                LinearIterator<Size> fartherst = find_farthest(current.first, current.second);
                if (fartherst == current.second)
                    continue;

                pending.emplace(fartherst, current.second);
                pending.emplace(current.first, fartherst);

                sorted_splits.emplace(std::distance(first, fartherst), fartherst);
            }

            for (auto& sorted_split : sorted_splits) {
                splits.push_back(sorted_split.second);
            }
        }

        void append_keys(
            LinearIterator<Size> first,
            LinearIterator<Size> last,
            std::list< LinearIterator<Size> > keys,
            Linear<Size>& gradient
        ) {
            gradient.reserve(gradient.size() + keys.size());

            float offset = out_range[0];
            float scale = (out_range[1] - out_range[0]);
            bool transform = out_range[0] != 0 || out_range[1] != 1;

            for (const auto& key : keys) {
                if (transform)
                    gradient.emplace_back(key->color, key->position * scale + offset);
                else
                    gradient.emplace_back(key->color, key->position);
            }
        }

        Linear<Size> build(
            LinearIterator<Size> first,
            LinearIterator<Size> last,
            std::list< LinearIterator<Size> > keys
        ) {
            if (std::distance(first, last) < 1)
                return {};

            Linear<Size> gradient;
            append_keys(first, last, keys, gradient);
            return gradient;
        }
    };

}
