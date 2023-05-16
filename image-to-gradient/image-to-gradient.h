#pragma once

#include "gradient.hpp"
#include "image.hpp"
#include "pixel.hpp"

namespace ItG {

    template<typename Layout>
    Gradient::Linear<Pixel::layout_size<Layout>::value> scan_line_to_linear(
        typename ImageFloat<Layout>::scan_line_t& scan_line,
        float start, float stop
    ) {
        using namespace Gradient;
        constexpr size_t Size = Pixel::layout_size<Layout>::value;

        if (scan_line.empty())
            return {};

        Linear<Size> gradient;
        gradient.reserve(scan_line.size());
        std::transform(scan_line.begin(), scan_line.end(), std::back_inserter(gradient), [&](auto& v) {
            return Key<Size>{ Pixel::to_color(v), gradient.size() / float(scan_line.size() - 1) };
        });

        LinearBuilder<Size> builder;
        builder.out_range = { start, stop };
        return builder.from_linear(gradient);
    }

    template<typename Layout>
    Gradient::Linear<Pixel::layout_size<Layout>::value> image_to_linear(ImageFloat<Layout>& image, float start, float stop) {
        if (!image.is_valid())
            return {};

        auto scan_line = image.scanLine(0);
        return scan_line_to_linear<Layout>(scan_line, start, stop);
    }

}