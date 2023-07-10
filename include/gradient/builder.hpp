#pragma once

#include <list>

#include "linear.hpp"

namespace ItG::Gradient {

    template<size_t Size>
    struct Builder {
        using TGradient = Linear<Size>;
        using TIterator = LinearIterator<Size>;

        std::array<float, 2> out_range{ { 0, 1} };

        void append_keys( TIterator first, TIterator last, std::list< TIterator > keys, TGradient& gradient ) {
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

        TGradient build( TIterator first, TIterator last, std::list< TIterator > keys ) {
            if (std::distance(first, last) < 1)
                return {};

            TGradient gradient;
            append_keys(first, last, keys, gradient);
            return gradient;
        }
    };

    template<size_t Size, typename StopExtractor>
    inline Linear<Size> from_gradient( Linear<Size>& gradient, Builder<Size>& builder, const StopExtractor& stop_extractor) {
        if (gradient.empty())
            return {};

        std::list<LinearIterator<Size>> keys;
        keys.push_back(gradient.begin());

        stop_extractor(gradient.begin(), gradient.end() - 1, keys);

        keys.push_back(gradient.end() - 1);

        return builder.build(gradient.begin(), gradient.end(), keys);
    }

    template<size_t Size, typename StopExtractor>
    inline Linear<Size> from_gradient( Linear<Size>& gradient, const StopExtractor& stop_extractor) {
        Builder<Size> builder{};
        return from_gradient(gradient, builder, stop_extractor);
    }

    template<size_t Size, typename StopExtractor>
    inline Linear<Size> from_colors( std::vector< Color<Size> >& colors, Builder<Size>& builder, const StopExtractor& stop_extractor) {
        if (colors.empty())
            return {};

        Linear<Size> gradient;
        gradient.reserve(colors.size());
        std::transform(colors.begin(), colors.end(), std::back_inserter(gradient), [&](auto& v) {
            return Key<Size>{ v, gradient.size() / float(colors.size() - 1) };
        });

        return from_gradient(gradient, builder, stop_extractor);
    }

    template<size_t Size, typename StopExtractor>
    inline Linear<Size> from_colors( std::vector< Color<Size> >& colors, const StopExtractor& stop_extractor) {
        Builder<Size> builder{};
        return from_colors(colors, builder, stop_extractor);
    }

}
