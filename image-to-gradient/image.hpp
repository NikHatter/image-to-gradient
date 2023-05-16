#pragma once

#include <filesystem>
#include <algorithm>
#include <ranges>

#include "boost/gil.hpp"
#include "boost/gil/extension/dynamic_image/any_image.hpp"
#include "boost/gil/extension/io/jpeg.hpp"
#include "boost/gil/extension/io/png.hpp"

namespace ItG {

    //ImageType = boost::gil::rgba8_image_t;
    template<typename ImageType>
    struct Image {
        using image_t = ImageType;
        using view_t = image_t::view_t;
        using pixel_t = image_t::value_type;
        using scan_line_t = std::vector<pixel_t>;

        Image() {}

        explicit Image(image_t& image) : image_data(image) {
            image_view = boost::gil::view(image_data);
        }

        inline bool is_valid() const { return width() > 0 && height() > 0; };

        inline ptrdiff_t width() const { return image_data.width(); };
        inline ptrdiff_t height() const { return image_data.height(); };

        inline ptrdiff_t unit_to_pixel(const float& u, const ptrdiff_t& size) {
            return std::clamp<ptrdiff_t>(static_cast<ptrdiff_t>(round(size * u)), 0, size - 1);
        }

        inline std::array<ptrdiff_t, 2> pixel(const float& x, const float& y) {
            return { { unit_to_pixel(x, width()), unit_to_pixel(x, height()) } };
        }

        inline pixel_t color(const ptrdiff_t& x, const ptrdiff_t& y) {
            return *image_view.xy_at(x, y);
        }

        inline void color(const ptrdiff_t& x, const ptrdiff_t& y, pixel_t& out_color) {
            out_color = *image_view.xy_at(x, y);
        }

        inline pixel_t color(const std::array<ptrdiff_t, 2>& xy) {
            return *image_view.xy_at(xy[0], xy[1]);
        }

        inline void color(const std::array<ptrdiff_t, 2>& xy, pixel_t& out_color) {
            out_color = *image_view.xy_at(xy[0], xy[1]);
        }

        inline scan_line_t scanLine(ptrdiff_t y) {
            scan_line_t line;
            line.reserve(width());
            for (ptrdiff_t x = 0; x < width(); x++) {
                line.emplace_back( color(x, y) );
            }
            return line;
        }

        const image_t& image() const { return image_data; };
        image_t& image() { return image_data; };

        const view_t& view() const { return image_view; };
        view_t& view() { return image_view; };

    protected:
        image_t image_data{};
        view_t image_view{};
    };

    template <typename Layout>
    using ImageFloat = Image< boost::gil::image< boost::gil::pixel<boost::gil::float32_t, Layout> > >;

    using LayoutRGB = boost::gil::rgb_layout_t;
    using LayoutRGBA = boost::gil::rgba_layout_t;

    using ImageRGB = ImageFloat<LayoutRGB>;
    using ImageRGBA = ImageFloat<LayoutRGBA>;

    template<typename image_t>
    Image<image_t> load(std::istream& stream) {
        image_t image{};
        try {
            boost::gil::read_and_convert_image(stream, image, boost::gil::png_tag{});
            return Image<image_t>(image);
        } catch (std::exception e) {

        }

        try {
            boost::gil::read_and_convert_image(stream, image, boost::gil::jpeg_tag{});
            return Image<image_t>(image);
        } catch (std::exception e) {

        }

        return {};
    }

    template<typename image_t>
    Image<image_t> load(const std::filesystem::path& path) {
        image_t image{};
        try {
            boost::gil::read_and_convert_image(path, image, boost::gil::png_tag{});
            return Image<image_t>(image);
        } catch (std::exception e) {

        }

        try {
            boost::gil::read_and_convert_image(path, image, boost::gil::jpeg_tag{});
            return Image<image_t>(image);
        } catch (std::exception e) {

        }

        return {};
    }
}
