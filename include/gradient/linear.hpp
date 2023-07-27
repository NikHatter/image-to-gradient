#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <vector>

#include "gradient/color.hpp"

namespace ItG::Gradient {

    /// @brief Default gradient key type template
    /// @tparam T Color type
    template<IsColor T>
    struct Key {
        /// @brief Number of channels of a color
        static constexpr size_t size = ColorSize<T>;
        /// @brief Color type
        using color_type = T;

        /// @brief Position in a gradient
        float position = 0.f;
        /// @brief Color value
        color_type color;

        Key() {
            color.fill(0.f);
        }
        Key(const color_type& color, const float& position) :  position(position), color(color)
        {}

        Key(const Key&) = default;
        Key(Key&&) = default;
        ~Key() = default;

        auto operator<=>(const Key&) const = default;
    };

    /// @brief Grayscale linear gradient with std::vector strage
    using LinearGray =  std::vector< Key<Color::Gray> >;
    /// @brief Grayscale with alpha linear gradient with std::vector strage
    using LinearGrayA = std::vector< Key<Color::GrayA> >;
    /// @brief RGB linear gradient with std::vector strage
    using LinearRGB =   std::vector< Key<Color::RGB> >;
    /// @brief RGBA linear gradient with std::vector strage
    using LinearRGBA =  std::vector< Key<Color::RGBA> >;
    /// @brief CMYK linear gradient with std::vector strage
    using LinearCMYK =  std::vector< Key<Color::CMYK> >;
    /// @brief CMYKA linear gradient with std::vector strage
    using LinearCMYKA = std::vector< Key<Color::CMYKA> >;

    /// @brief Type can be used as key in gradient algorithms
    template<typename T>
    concept IsKey = requires (T x) { 
        T::size;
        typename T::color_type;
        requires IsColorN<typename T::color_type, T::size>;
        { x.color } -> std::convertible_to<typename T::color_type>;
        { x.position } -> std::convertible_to<float>;
    };

    /// @brief Linear gradient type for input
    template<typename T>
    concept Linear = requires {
        typename T::value_type;
        requires IsKey<typename T::value_type>;
    };

    /// @brief Linear gradient type for input/output
    template<typename T>
    concept LinearData = Linear<T> && requires (T & x) { x.emplace_back(typename T::value_type{}); };

    /// @brief Linear gradient iterator type
    template<typename T>
    concept LinearIterator = Linear<T> && std::input_iterator<T>;

    /// @brief Linear gradient range type
    template<typename T>
    concept LinearRange = Linear<T> && std::ranges::range<T> || std::ranges::view<T> && requires (T x) { { x.begin() } -> LinearIterator; };


    /// @brief Deduce iterator for LinearRange
    template<LinearRange T>
    using LinearRange_Iterator = decltype(std::begin(T{}));

    /// @brief Deduce key type for LinearRange
    template<LinearRange T>
    using LinearRange_Value = LinearRange_Iterator<T>::value_type;

    /// @brief Deduce color type for LinearRange
    template<LinearRange T>
    using LinearRange_Color = LinearRange_Value<T>::color_type;

    /// @brief Deduce subrange type for LinearRange
    template<LinearRange T>
    using LinearRange_Subrange = decltype(std::ranges::subrange(LinearRange_Iterator<T>{}, LinearRange_Iterator<T>{}));


    /// @brief Linear gradients of colors with same channel count
    template<typename T, typename U>
    concept SameSize = Linear<T> && Linear<U> && requires { requires (T::value_type::size == U::value_type::size); };

    /// @brief Linear gradient of colors with N channels
    template<typename T, size_t N>
    concept OfSize = Linear<T> && requires { requires (T::value_type::size == N); };


    static_assert(IsKey<Key<Color::Gray>>,  "Key<Color::Gray> is not a gradient key!");
    static_assert(IsKey<Key<Color::GrayA>>, "Key<Color::GrayA> is not a gradient key!");
    static_assert(IsKey<Key<Color::RGB>>,   "Key<Color::RGB> is not a gradient key!");
    static_assert(IsKey<Key<Color::RGBA>>,  "Key<Color::RGBA> is not a gradient key!");
    static_assert(IsKey<Key<Color::CMYK>>,  "Key<Color::CMYK> is not a gradient key!");
    static_assert(IsKey<Key<Color::CMYKA>>, "Key<Color::CMYKA> is not a gradient key!");

    static_assert(Linear<LinearGray>,  "LinearGray is not a linear graident!");
    static_assert(Linear<LinearGrayA>, "LinearGrayA is not a linear graident!");
    static_assert(Linear<LinearRGB>,   "LinearRGB is not a linear graident!");
    static_assert(Linear<LinearRGBA>,  "LinearRGBA is not a linear graident!");
    static_assert(Linear<LinearCMYK>,  "LinearCMYK is not a linear graident!");
    static_assert(Linear<LinearCMYKA>, "LinearCMYKA is not a linear graident!");

    static_assert(OfSize<LinearGray, 1>,  "LinearGray color size is not 1!");
    static_assert(OfSize<LinearGrayA, 2>, "LinearGrayA color size is not 2!");
    static_assert(OfSize<LinearRGB, 3>,   "LinearRGB color size is not 3!");
    static_assert(OfSize<LinearRGBA, 4>,  "LinearRGBA color size is not 4!");
    static_assert(OfSize<LinearCMYK, 4>,  "LinearCMYK color size is not 4!");
    static_assert(OfSize<LinearCMYKA, 5>, "LinearCMYKA color size is not 5!");
}
