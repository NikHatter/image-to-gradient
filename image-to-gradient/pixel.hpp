#pragma once

#include <algorithm>

#include "boost/gil/typedefs.hpp"
#include "boost/gil/pixel.hpp"
#include "boost/gil/color_base_algorithm.hpp"

namespace ItG::Pixel {

    template <typename Result>
    struct OperatorDiff {
        template <typename T1, typename T2>
        Result operator()(T1 a, T2 b) const { return static_cast<Result>(std::abs( a - b ) ); }
    };

    template <typename Result>
    struct OperatorLerp {
        float Ratio = 0.5;

        OperatorLerp() {}
        explicit OperatorLerp(const float& ratio) : Ratio(ratio) {}

        template <typename T1, typename T2>
        Result operator()(T1 a, T2 b) const {
            return Result( std::lerp<float>(float(a), float(b), Ratio) );
        }
    };

    template <typename PixelT>
    PixelT diff(const PixelT& a, const PixelT& b) {
        using result_channel_t = typename boost::gil::channel_type<PixelT>::type;

        PixelT result{};
        boost::gil::static_transform(a, b, result, OperatorDiff<result_channel_t>());
        return result;
    }

    template<typename ChannelT, typename LayoutT>
    boost::gil::pixel<boost::gil::float32_t, LayoutT> lerp(const boost::gil::pixel<ChannelT, LayoutT>& a, const boost::gil::pixel<ChannelT, LayoutT>& b, const float& u) {
        boost::gil::pixel<boost::gil::float32_t, LayoutT> result{};
        boost::gil::static_transform(a, b, result, OperatorLerp<float>(u));
        return result;
    }
}
