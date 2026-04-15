#pragma once

#ifndef META_H
#define META_H

#include <type_traits>
#include <cstdint>

namespace Meta

{
    template <typename T, T VAL>
    struct constant { static constexpr T value { VAL }; };

    struct true_type  : constant <bool, true> {};
    struct false_type : constant <bool, false>{};



    template <typename T, typename U>
    struct is_same : false_type {};
    template <typename T>
    struct is_same<T,T> : true_type {};
    template <typename T, typename U>
    constexpr bool is_same_v = is_same<T,U>::value;


    template <typename T>
    struct type_id { using type = T; };


    template <std::size_t N, typename... Ts>
    struct nth_type { static_assert(sizeof...(Ts) != 0); };
    template <std::size_t N, typename... Ts>
    using nth_type_t = typename nth_type<N, Ts...>::type;
    template <typename T, typename... Ts>
    struct nth_type<0, T, Ts...> : type_id < T > {};
    template <std::size_t N, typename T, typename... Ts>
    struct nth_type<N, T, Ts...> : type_id < nth_type_t< N-1, Ts...>> {};

    

    template <typename T, typename... Ts>
    struct pos_type { static_assert(sizeof...(Ts) != 0); };
    template <typename T, typename... Ts>
    constexpr std::size_t pos_type_v = pos_type<T, Ts...>::value;
    template <typename T, typename... Ts>
    struct pos_type<T, T, Ts...> : constant < std::size_t, 0 > {};
    template <typename T, typename U, typename... Ts>
    struct pos_type<T, U, Ts...> : constant < std::size_t, 1 + pos_type_v< T, Ts...> > {};


    template <bool Cond, typename T, typename F>
    struct IFT : type_id < F > {};
    template <typename T, typename F>
    struct IFT<true, T, F> : type_id < T > {};
    template <bool Cond, typename T, typename F>
    using IFT_t = typename IFT<Cond, T, F>::type;


    template <typename...Ts>
    struct TypeList{

        consteval static std::size_t size() noexcept {
            return sizeof...(Ts);
        };

        template <typename T>
        consteval static bool contains() noexcept{
            return (false || ... || is_same_v<T, Ts>);
        }

        template <typename T>
        consteval static std::size_t pos() noexcept{
            static_assert(contains<T>());
            return pos_type_v<T, Ts...>;
        }

    };


    template <template <typename...> class New, typename List>
    struct replace{};
    template <template <typename...> class New, typename... Ts>
    struct replace<New, TypeList<Ts...>> : type_id < New<Ts...> > {};
    template <template <typename...> class New, typename List>
    using replace_t = typename replace<New, List>::type;
};

#include "Meta.tpp"

#endif