#pragma once

#ifndef META_TPP
#define META_TPP

#include "Meta.hpp"
#include "MetaFWD.hpp"
#include "../man/Slotmap.hpp"

namespace Meta {
    template <typename... Ts>
    struct ToSlotmap<TypeList<Ts...>> {
        using type = std::tuple<Slotmap<Ts, Ts::Capacity>...>;
    };

    template <typename... Ts>
    struct ToComponentTuple<TypeList<Ts...>> {
        using type = std::tuple<std::optional<typename Slotmap<Ts, Ts::Capacity>::key_type>...>;
    };
}

#endif
