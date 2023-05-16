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
    template<typename image_type>
    struct Image {
        using ImageType = image_type;
        using ViewType = ImageType::view_t;
        using PixelType = ImageType::value_type;
        using ScanLine = std::vector<PixelType>;

        Image() {}

        explicit Image(ImageType& image) : image_data(image) {
            image_view = boost::gil::view(image_data);
        }

        inline bool isValid() const { return width() > 0 && height() > 0; };

        inline ptrdiff_t width() const { return image_data.width(); };
        inline ptrdiff_t height() const { return image_data.height(); };

        inline ptrdiff_t unitToPixel(const float& u, const ptrdiff_t& size) {
            return std::clamp<ptrdiff_t>(static_cast<ptrdiff_t>(round(size * u)), 0, size - 1);
        }

        inline std::array<ptrdiff_t, 2> pixel(const float& x, const float& y) {
            return { { unitToPixel(x, width()), unitToPixel(x, height()) } };
        }

        inline PixelType color(const ptrdiff_t& x, const ptrdiff_t& y) {
            return *image_view.xy_at(x, y);
        }

        inline void color(const ptrdiff_t& x, const ptrdiff_t& y, PixelType& out_color) {
            out_color = *image_view.xy_at(x, y);
        }

        inline PixelType color(const std::array<ptrdiff_t, 2>& xy) {
            return *image_view.xy_at(xy[0], xy[1]);
        }

        inline void color(const std::array<ptrdiff_t, 2>& xy, PixelType& out_color) {
            out_color = *image_view.xy_at(xy[0], xy[1]);
        }

        inline ScanLine scanLine(ptrdiff_t y) {
            ScanLine line;
            line.reserve(width());
            for (ptrdiff_t x = 0; x < width(); x++) {
                line.emplace_back( color(x, y) );
            }
            return line;
        }

        const ImageType& image() const { return image_data; };
        ImageType& image() { return image_data; };

        const ViewType& view() const { return image_view; };
        ViewType& view() { return image_view; };

    protected:
        ImageType image_data{};
        ViewType image_view{};
    };

    template <typename Layout>
    using ImageFloat = Image< boost::gil::image< boost::gil::pixel<boost::gil::float32_t, Layout> > >;

    using LayoutRGB = boost::gil::rgb_layout_t;
    using LayoutRGBA = boost::gil::rgba_layout_t;

    using ImageRGB = ImageFloat<LayoutRGB>;
    using ImageRGBA = ImageFloat<LayoutRGBA>;

    template<typename ImageType>
    Image<ImageType> load(std::istream& stream) {
        ImageType image{};
        try {
            boost::gil::read_and_convert_image(stream, image, boost::gil::png_tag{});
            return Image<ImageType>(image);
        } catch (std::exception e) {

        }

        try {
            boost::gil::read_and_convert_image(stream, image, boost::gil::jpeg_tag{});
            return Image<ImageType>(image);
        } catch (std::exception e) {

        }

        return {};
    }

    template<typename ImageType>
    Image<ImageType> load(const std::filesystem::path& path) {
        ImageType image{};
        try {
            boost::gil::read_and_convert_image(path, image, boost::gil::png_tag{});
            return Image<ImageType>(image);
        } catch (std::exception e) {

        }

        try {
            boost::gil::read_and_convert_image(path, image, boost::gil::jpeg_tag{});
            return Image<ImageType>(image);
        } catch (std::exception e) {

        }

        return {};
    }
}
