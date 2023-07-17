#pragma once

#include <list>
#include <span>
#include <ranges>

#include "linear.hpp"

namespace ItG::Gradient {

    template<LinearData TGradient>
    struct Builder {
        using TIterator = typename TGradient::iterator;

        std::array<float, 2> out_range{ { 0, 1} };

        void append_keys(TGradient& keys, TGradient& gradient ) const {
            gradient.reserve(gradient.size() + keys.size());

            const float offset = out_range[0];
            const float scale = (out_range[1] - out_range[0]);
            const bool transform = out_range[0] != 0 || out_range[1] != 1;
            
            if (transform) {
                std::ranges::transform(keys, back_inserter(gradient), [&](const auto& key) { return LinearRange_Value<TGradient>{key.color, key.position * scale + offset}; });
            } else {
                std::ranges::copy(keys, back_inserter(gradient));
            }
        }

        [[nodiscard]] TGradient build(TGradient& keys ) const {
            if (std::size(keys) < 1)
                return {};

            TGradient gradient;
            append_keys(keys, gradient);
            return gradient;
        }
    };

    template<typename DistanceOp, typename Strategy, LinearData TGradient>
    inline [[nodiscard]] TGradient from_gradient( TGradient& gradient, Builder<TGradient>& builder, Strategy&& strategy = {}) {
        using Iterator = Builder<TGradient>::TIterator;

        if (gradient.empty())
            return {};

        TGradient keys;
        keys.push_back(gradient.front());

        strategy(std::ranges::subrange(gradient.begin(), gradient.end()), keys, DistanceOp{});

        keys.push_back(gradient.back());

        return builder.build(keys);
    }

    template<typename DistanceOp, typename Strategy, LinearData TGradient>
    inline [[nodiscard]] TGradient from_gradient( TGradient& gradient, Strategy&& strategy = {}) {
        Builder<TGradient> builder{};
        return from_gradient<DistanceOp>(gradient, builder, std::forward<Strategy>(strategy));
    }

    template<typename DistanceOp, typename Strategy, LinearData TGradient>
    inline [[nodiscard]] TGradient from_colors( std::vector< LinearRange_Value<TGradient> >& colors, Builder<TGradient>& builder, Strategy&& strategy = {}) {
        using KeyType = TGradient::value_type;

        if (colors.empty())
            return {};

        TGradient gradient;
        gradient.reserve(colors.size());
        std::ranges::transform(colors, std::back_inserter(gradient), [&](auto& v) {
            return KeyType{ v, gradient.size() / float(colors.size() - 1) };
        });

        return from_gradient<DistanceOp>(gradient, builder, std::forward<Strategy>(strategy));
    }

    template<typename DistanceOp, typename Strategy, LinearData TGradient>
    inline [[nodiscard]] TGradient from_colors(std::vector< LinearRange_Value<TGradient> >& colors, Strategy&& strategy = {}) {
        Builder<TGradient> builder{};
        return from_colors<DistanceOp>(colors, builder, std::forward<Strategy>(strategy));
    }

}
