#pragma once

#ifndef ENTITY_TPP
#define ENTITY_TPP

#include "Entity.hpp"
#include "../util/Meta.hpp"  // Ahora sí incluimos Meta.hpp

template <typename CMPS, typename TAGS>
Entity<CMPS, TAGS>::Entity(EManager& parent) : parent_(&parent) {}

template <typename CMPS, typename TAGS>
EManager& Entity<CMPS, TAGS>::getParent() {
    return *parent_;
}

template <typename CMPS, typename TAGS>
const EManager& Entity<CMPS, TAGS>::getParent() const {
    return *parent_;
}

template <typename CMPS, typename TAGS>
template <typename T>
void Entity<CMPS, TAGS>::setComponentKey(typename Slotmap<T, T::Capacity>::key_type key) {
    constexpr size_t index = CMPS::template pos<T>();
    std::get<index>(componentKeys) = key;
}

template <typename CMPS, typename TAGS>
template <typename T>
std::optional<typename Slotmap<T, T::Capacity>::key_type> Entity<CMPS, TAGS>::getComponentKey() const {
    constexpr size_t index = CMPS::template pos<T>();
    return std::get<index>(componentKeys);
}

#endif