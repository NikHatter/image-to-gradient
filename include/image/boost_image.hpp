#pragma once

#include <filesystem>
#include <algorithm>
#include <ranges>

#include "boost_pixel.hpp"
#include "gradient/linear.hpp"

#include "boost/gil.hpp"
#include "boost/gil/extension/dynamic_image/any_image.hpp"
#include "boost/gil/extension/io/jpeg.hpp"
#include "boost/gil/extension/io/png.hpp"

namespace ItG::Image::gil {

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
    inline std::vector< std::array<float, view_size<View>::value> > get_colors(View& view, const float& x1, const float& y1, const float& x2, const float& y2, const ptrdiff_t& count) {
        constexpr size_t Size = view_size<View>::value;

        if (!is_valid(view))
            return {};

        std::vector<std::array<float, Size>> colors;
        colors.reserve(count);

        const ptrdiff_t width = view.width();
        const ptrdiff_t height = view.height();
        const float step_x = (x2 - x1) / (count - 1);
        const float step_y = (y2 - y1) / (count - 1);
        float sample_x = x1;
        float sample_y = y1;

        for (ptrdiff_t i_sample = 0; i_sample < count; i_sample++, sample_x += step_x, sample_y += step_y) {
            colors.emplace_back( to_color(
                *view.xy_at( unit_to_point(sample_x, sample_y, width, height) )
            ) );
        }
        return colors;
    }

    template<typename TGradient, typename View> requires Gradient::OfSize<TGradient, view_size<View>::value>
    inline TGradient get_linear(View& view, float x1, float y1, float x2, float y2) {
        constexpr size_t Size = view_size<View>::value;

        if (!is_valid(view))
            return {};

        TGradient gradient;

        const ptrdiff_t width = view.width();
        const ptrdiff_t height = view.height();

        ptrdiff_t i_x1 = unit_to_pixel(x1, width);
        ptrdiff_t i_x2 = unit_to_pixel(x2, width);
        ptrdiff_t i_y1 = unit_to_pixel(y1, height);
        ptrdiff_t i_y2 = unit_to_pixel(y2, height);

        ptrdiff_t size_x = i_x2 - i_x1;
        ptrdiff_t size_y = i_y2 - i_y1;
        gradient.reserve(std::max(abs(size_x), abs(size_y)));

        ptrdiff_t sample_x = i_x1;
        ptrdiff_t sample_y = i_y1;
        float position = 0.f;

        if (std::max(abs(size_x), abs(size_y)) < 3 ) {
            gradient.emplace_back(to_color( *view.xy_at( i_x1, i_y1 ) ), 0.f);
            gradient.emplace_back(to_color( *view.xy_at( i_x2, i_y2 ) ), 1.f);
        } else if (abs(size_x) >= abs(size_y)) {
            for (int step = (size_x > 0 ? 1 : -1); sample_x != i_x2;  sample_x += step) {
                position = fabs(float(sample_x - i_x1) / size_x);
                sample_y = static_cast<int>(std::lerp(i_y1, i_y2, position));

                gradient.emplace_back(to_color( *view.xy_at(sample_x, sample_y) ), position);
            }
        } else if (abs(size_x) < abs(size_y)) {
            for (int step = (size_y > 0 ? 1 : -1); sample_y != i_y2;  sample_y += step) {
                position = fabs(float(sample_y - i_y1) / size_y);
                sample_x = static_cast<int>(std::lerp(i_x1, i_x2, position));

                gradient.emplace_back(to_color( *view.xy_at(sample_x, sample_y) ), position);
            }
        }
        return gradient;
    }

}
