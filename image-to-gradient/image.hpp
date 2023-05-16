#pragma once

#include <filesystem>
#include <algorithm>
#include <ranges>

#include "pixel.hpp"

#include "boost/gil.hpp"
#include "boost/gil/extension/dynamic_image/any_image.hpp"
#include "boost/gil/extension/io/jpeg.hpp"
#include "boost/gil/extension/io/png.hpp"

namespace ItG::Image {

    template <typename Layout>
    using ImageFloat = boost::gil::image< boost::gil::pixel<boost::gil::float32_t, Layout> >;

    using LayoutRGB = boost::gil::rgb_layout_t;
    using LayoutRGBA = boost::gil::rgba_layout_t;

    using RGB = ImageFloat<LayoutRGB>;
    using RGBA = ImageFloat<LayoutRGBA>;


    template<typename View>
    inline bool is_valid(const View& view) { return view.width() > 0 && view.height() > 0; };

    inline ptrdiff_t unit_to_pixel(const float& u, const ptrdiff_t& size) {
        return std::clamp<ptrdiff_t>(static_cast<ptrdiff_t>(round(size * u)), 0, size - 1);
    }

    inline boost::gil::point_t unit_to_point(const float& u, const float& v, const ptrdiff_t& width, const ptrdiff_t& height) {
        return { unit_to_pixel(u, width), unit_to_pixel(v, height) };
    }

    template<typename image_t>
    image_t load(std::istream& stream) {
        image_t image{};
        try {
            boost::gil::read_and_convert_image(stream, image, boost::gil::png_tag{});
            return image;
        } catch (std::exception e) {

        }

        try {
            boost::gil::read_and_convert_image(stream, image, boost::gil::jpeg_tag{});
            return image;
        } catch (std::exception e) {

        }

        return image;
    }

    template<typename image_t>
    image_t load(const std::filesystem::path& path) {
        image_t image{};
        try {
            boost::gil::read_and_convert_image(path, image, boost::gil::png_tag{});
            return image;
        } catch (std::exception e) {

        }

        try {
            boost::gil::read_and_convert_image(path, image, boost::gil::jpeg_tag{});
            return image;
        } catch (std::exception e) {

        }

        return image;
    }

    template<typename View>
    inline std::vector< Color<Pixel::view_size<View>::value> > get_colors(View& view, const float& x1, const float& y1, const float& x2, const float& y2, const ptrdiff_t& count) {
        constexpr size_t Size = Pixel::view_size<View>::value;

        if (!is_valid(view))
            return {};

        std::vector<Color<Size>> colors;
        colors.reserve(count);

        const ptrdiff_t width = view.width();
        const ptrdiff_t height = view.height();
        const float step_x = (x2 - x1) / (count - 1);
        const float step_y = (y2 - y1) / (count - 1);
        float sample_x = x1;
        float sample_y = y1;

        for (ptrdiff_t i_sample = 0; i_sample < count; i_sample++, sample_x += step_x, sample_y += step_y) {
            colors.emplace_back( Pixel::to_color(
                *view.xy_at( unit_to_point(sample_x, sample_y, width, height) )
            ) );
        }
        return colors;
    }

}
