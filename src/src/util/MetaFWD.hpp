#pragma once

#ifndef METAFWD_H
#define METAFWD_H

#include <tuple>
#include <optional>

namespace Meta {
    template <typename... Ts>
    struct TypeList;

    template <typename List>
    struct ToSlotmap;
    
    template <typename List>
    struct ToComponentTuple;

    template <typename List>
    using ToSlotmap_t = typename ToSlotmap<List>::type;

    template <typename List>
    using ToComponentTuple_t = typename ToComponentTuple<List>::type;
}

#endif
