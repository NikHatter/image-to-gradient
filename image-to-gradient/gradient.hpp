// image-to-gradient.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <array>
#include <list>
#include <limits>

#include "pixel.hpp"
#include "image.hpp"

namespace ItG {

    template<typename Layout>
    struct GradientKey {
        boost::gil::pixel<boost::gil::float32_t, Layout> color{ 0 };
        float position{ 0 };
    };

    template<typename Layout>
    using Gradient = std::vector<GradientKey<Layout>>;

    template<typename Layout>
    struct GradientPositionLess {
        bool operator()(const GradientKey<Layout>& a, const GradientKey<Layout>& b) const {
            return a.position < b.position;
        }
        bool operator()(const GradientKey<Layout>& a, const float& b) const {
            return a.position < b;
        }
        bool operator()(const float& a, const GradientKey<Layout>& b) const {
            return a < b.position;
        }
    };

    //using Pixel = boost::gil::rgba8_pixel_t;
    template<typename Layout>
    struct MaxDeflection {
        using PixelType = boost::gil::pixel<boost::gil::float32_t, Layout>;
        using VectorType = std::vector<PixelType>;
        using IteratorType = VectorType::const_iterator;

        const float default_tolerance = 4.f / 255.f;

        static float deflection(const IteratorType& first, const IteratorType& last, const IteratorType& a, const float& u) {
            auto expected = Pixel::lerp(*first, *last, u);
            auto diff = Pixel::diff(expected, *a);
            float max_diff = boost::gil::static_max( Pixel::diff(expected, *a) );
            return static_cast<float>(max_diff);
        }

        static float deflection(const PixelType& a, const PixelType& b) {
            return boost::gil::static_max( Pixel::diff(a, b) );
        }

        static bool same(const PixelType& a, const PixelType& b, const float& tolerance) {
            return deflection(a, b) <= tolerance;
        }

        static void linear_split(IteratorType first, IteratorType last, std::list<IteratorType>& splits, float tolerance) {
            last--;

            IteratorType start = first;
            if (start == last) {
                splits.push_back(start);
                splits.push_back(start);
                return;
            }

            std::array<IteratorType, 4> checks{ { std::next(start),std::next(start),std::next(start),std::next(start) } };
            bool same_s_m = same(*start, *checks[0], tolerance);
            if (checks[0] == last) {
                splits.push_back(start);
                splits.push_back(checks[0]);
                return;
            }

            IteratorType end = std::next(checks[0]);
            if (end == last) {
                bool same_m_e = same(*end, *checks[0], tolerance);
                splits.push_back(start);
                if (!same_s_m || !same_m_e)
                    splits.push_back(checks[0]);
                splits.push_back(end);
                return;
            }

            size_t next_dist = 2;
            std::array<float, 4> checks_deflection{};
            std::array<float, 4> checks_u{ 0.5, 0.5, 0.5, 0.5 };

            while (end != last) {
                checks_deflection[0] = deflection(start, end, checks[0], checks_u[0]);
                checks_deflection[1] = deflection(start, end, checks[1], checks_u[1]);
                checks_deflection[2] = deflection(start, end, checks[2], checks_u[2]);
                checks_deflection[3] = deflection(start, end, checks[3], checks_u[3]);

                auto step_deflection = deflection(*std::prev(end), *std::next(end));
                bool by_next = step_deflection > tolerance;
                bool by_middle = checks_deflection[0] > tolerance || checks_deflection[1] > tolerance || checks_deflection[2] > tolerance || checks_deflection[3] > tolerance;

                if (by_next || by_middle) {
                    if (splits.empty()) {
                        splits.push_back(start);
                    }
                    splits.push_back(end);

                    start = end;
                    end = std::next(start);
                    if (end == last)
                        break;

                    next_dist = 1;
                }

                next_dist++;
                checks[0] = start + static_cast<size_t>(ceil(next_dist * 0.25));
                checks[1] = start + static_cast<size_t>(next_dist * 0.5);
                checks[2] = start + static_cast<size_t>(floor(next_dist * 0.75));
                checks[3] = end;
                checks_u[0] = std::distance(start, checks[0]) / float(next_dist);
                checks_u[1] = std::distance(start, checks[1]) / float(next_dist);
                checks_u[2] = std::distance(start, checks[2]) / float(next_dist);
                checks_u[3] = std::distance(start, checks[3]) / float(next_dist);

                end++;
            }

            IteratorType split = last;
            if (std::distance(start, split) > 0) {
                if (!same(*start, *split, tolerance)) {
                    splits.push_back(split);
                }
            }

            if (splits.empty()) {
                splits.push_back(start);
                splits.push_back(split);
            }

            return;
        }

        static void recurse_split(IteratorType first, IteratorType last, std::list<IteratorType>& splits, float tolerance, bool left = false) {
            IteratorType fartherst = std::next(first);
            if (fartherst == last) {
                return;
            }

            float size = std::distance(first, last) - 1.f;
            auto max_dist = deflection(first, last, fartherst, std::distance(first, fartherst) / size);
            for (IteratorType color = fartherst + 1; color != last; ++color) {
                auto cur_dist = deflection(first, last, color, std::distance(first, color) / size);
                if (cur_dist > max_dist || !left && cur_dist == max_dist) {
                    max_dist = cur_dist;
                    fartherst = color;
                }
            }

            if (max_dist < tolerance)
                return;

            recurse_split(first, fartherst, splits, tolerance, true);

            auto prev = fartherst - 1;
            auto next = fartherst + 1;

            splits.push_back(fartherst);

            recurse_split(fartherst, last, splits, tolerance);
        }

    };

    template<typename Layout>
    void toGradient(
        typename ImageFloat<Layout>::ScanLine::const_iterator first,
        typename ImageFloat<Layout>::ScanLine::const_iterator last,
        std::list<typename ImageFloat<Layout>::ScanLine::const_iterator> keys,
        Gradient<Layout>& gradient,
        float start, float stop
    ) {
        gradient.reserve(gradient.size() + keys.size());

        float offset = start;
        float scale = (stop - start);
        bool transform = start != 0 || stop != 1;

        float max_index = std::distance(first, last) - 1.f;
        ptrdiff_t last_index = 0;
        for (const auto& key : keys) {
            auto index = std::distance(first, key);

            float position = index / max_index;
            // for bigger images assume very close indices to be at band ends only
            if (max_index > 64 && index - last_index < 3) {
                position = (last_index / max_index);
            }
            if (transform)
                gradient.emplace_back(*key, position * scale + offset);
            else
                gradient.emplace_back(*key, position);

            last_index = index;
        }
    }

    template<typename Layout>
    Gradient<Layout> toGradient(
        typename ImageFloat<Layout>::ScanLine::const_iterator first,
        typename ImageFloat<Layout>::ScanLine::const_iterator last,
        std::list<typename ImageFloat<Layout>::ScanLine::const_iterator> keys,
        float start, float stop
    ) {
        if (std::distance(first, last) < 1)
            return {};

        Gradient<Layout> gradient;
        toGradient<Layout>(first, last, keys, gradient, start, stop);
        return gradient;
    }

    template<typename Layout>
    Gradient<Layout> toGradient(
        typename ImageFloat<Layout>::ScanLine& scan_line,
        float start, float stop
    ) {
        if (scan_line.empty())
            return {};

        Gradient<Layout> gradient;
        MaxDeflection<Layout> splitter;

        std::list<typename MaxDeflection<Layout>::IteratorType> keys;
        keys.push_back(scan_line.begin());
        splitter.recurse_split(scan_line.begin(), scan_line.end() - 1, keys, splitter.default_tolerance);
        keys.push_back(scan_line.end() - 1);
        //MaxDeflection<Layout>::linear_split(scan_line.begin(), scan_line.end() - 1, keys);
        return toGradient<Layout>(scan_line.begin(), scan_line.end(), keys, start, stop);
    }

    template<typename Layout>
    Gradient<Layout> toGradient(ImageFloat<Layout>& image, float start, float stop) {
        if (!image.isValid())
            return {};

        auto scan_line = image.scanLine(0);
        return toGradient<Layout>(scan_line, start, stop);
    }

}
