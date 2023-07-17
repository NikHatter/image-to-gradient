#pragma once

#include <QImage>
#include "gradient/linear.hpp"


namespace ItG::Image::Qt {

    inline ItG::Color::RGBA to_color(const QRgb& value) {
        constexpr float scale = 1.f / 255.f;
        return { qRed(value) * scale, qGreen(value) * scale, qBlue(value) * scale, qAlpha(value) * scale };
    }


    inline QColor get_color(const QImage& image, float x, float y) {
        return image.pixelColor(
            static_cast<int>(x * (image.width() - 1)), static_cast<int>(y * (image.height() - 1))
        );
    }

    inline ItG::Gradient::LinearRGBA get_linear(const QImage& image, float x1, float y1, float x2, float y2) {
        if (image.isNull())
            return {};

        ItG::Gradient::LinearRGBA gradient;

        const int width = image.width();
        const int height = image.height();

        int i_x1 = static_cast<int>(x1 * (width - 1));
        int i_x2 = static_cast<int>(x2 * (width - 1));
        int i_y1 = static_cast<int>(y1 * (height - 1));
        int i_y2 = static_cast<int>(y2 * (height - 1));

        int size_x = i_x2 - i_x1;
        int size_y = i_y2 - i_y1;
        gradient.reserve(std::max(abs(size_x), abs(size_y)));

        int sample_x = i_x1;
        int sample_y = i_y1;
        float position = 0.f;

        if (std::max(abs(size_x), abs(size_y)) < 3) {
            gradient.emplace_back(to_color(image.pixel(i_x1, i_y1)), 0.f);
            gradient.emplace_back(to_color(image.pixel(i_x2, i_y2)), 1.f);
        } else if (abs(size_x) >= abs(size_y)) {
            for (int step = (size_x > 0 ? 1 : -1); sample_x != i_x2;  sample_x += step) {
                position = fabs(float(sample_x - i_x1) / size_x);
                sample_y = static_cast<int>(std::lerp(i_y1, i_y2, position));

                gradient.emplace_back(to_color(image.pixel(sample_x, sample_y)), position);
            }
        } else if (abs(size_x) < abs(size_y)) {
            for (int step = (size_y > 0 ? 1 : -1); sample_y != i_y2;  sample_y += step) {
                position = fabs(float(sample_y - i_y1) / size_y);
                sample_x = static_cast<int>(std::lerp(i_x1, i_x2, position));
                    
                gradient.emplace_back(to_color(image.pixel(sample_x, sample_y)), position);
            }
        }
        return gradient;
    }

}
