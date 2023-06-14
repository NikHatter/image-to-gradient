#pragma once

#include "gradient/color.hpp"

#include "boost/gil/typedefs.hpp"
#include "boost/gil/pixel.hpp"

namespace ItG::Pixel {

    template<typename Layout>
    using layout_size = boost::mp11::mp_size< typename Layout::color_space_t >;

    template<typename Pixel>
    using pixel_size = layout_size< typename Pixel::layout_t >;

    template<typename View>
    using view_size = pixel_size< typename View::value_type >;

    template <typename Channel, typename Layout>
    Color<layout_size<Layout>::value> to_color(const boost::gil::pixel<Channel, Layout>& value) {
        constexpr size_t Size = layout_size<Layout>::value;

        Color<Size> result{};
        for (size_t i = 0; i < Size; i++) {
            if constexpr (std::is_integral_v<Channel>) {
                result[i] = float(value[i]) / std::numeric_limits<Channel>::max();
            } else {
                result[i] = value[i];
            }
        }
        return result;
    }
}
