#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <vector>

#include "gradient/color.hpp"

namespace ItG::Gradient {

    template<IsColor T>
    struct Key {
        static constexpr size_t size = ColorSize<T>;
        using color_type = T;

        float position = 0.f;
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

    using LinearGray =  std::vector< Key<Color::Gray> >;
    using LinearGrayA = std::vector< Key<Color::GrayA> >;
    using LinearRGB =   std::vector< Key<Color::RGB> >;
    using LinearRGBA =  std::vector< Key<Color::RGBA> >;
    using LinearCMYK =  std::vector< Key<Color::CMYK> >;
    using LinearCMYKA = std::vector< Key<Color::CMYKA> >;

    template<typename T>
    concept IsKey = requires (T x) { 
        T::size;
        typename T::color_type;
        requires IsColorN<typename T::color_type, T::size>;
        { x.color } -> std::convertible_to<typename T::color_type>;
        { x.position } -> std::convertible_to<float>;
    };

    template<typename T>
    concept Linear = requires {
        typename T::value_type;
        requires IsKey<typename T::value_type>;
    };

    template<typename T>
    concept LinearData = Linear<T> && requires (T & x) { x.emplace_back(typename T::value_type{}); };

    template<typename T>
    concept LinearIterator = Linear<T> && std::input_iterator<T>;

    template<typename T>
    concept LinearRange = Linear<T> && std::ranges::range<T> || std::ranges::view<T> && requires (T x) { { x.begin() } -> LinearIterator; };


    template<LinearRange T>
    using LinearRange_Iterator = decltype(std::begin(T{}));

    template<LinearRange T>
    using LinearRange_Value = LinearRange_Iterator<T>::value_type;

    template<LinearRange T>
    using LinearRange_Color = LinearRange_Value<T>::color_type;

    template<LinearRange T>
    using LinearRange_Subrange = decltype(std::ranges::subrange(LinearRange_Iterator<T>{}, LinearRange_Iterator<T>{}));


    template<typename T, typename U>
    concept SameSize = Linear<T> && Linear<U> && requires { requires (T::value_type::size == U::value_type::size); };

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
