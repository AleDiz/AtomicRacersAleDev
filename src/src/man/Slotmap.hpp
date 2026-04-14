#pragma once

#ifndef SLOTMAP_H
#define SLOTMAP_H

#include <array>
#include <stdexcept>
#include <cstdint>
#include <cassert>

template <typename DataType, std::size_t Capacity = 10, typename INDEXT = std::uint32_t>
class Slotmap {
public:
    using value_type = DataType;
    using index_type = INDEXT;
    using gen_type   = index_type;
    using key_type   = struct { index_type id; gen_type gen; };
    using iterator   = value_type*;
    using const_iterator = value_type const*;

    constexpr explicit Slotmap();
    
    [[nodiscard]] constexpr std::size_t     size() const noexcept;
    [[nodiscard]] constexpr std::size_t     capacity() const noexcept;
    [[nodiscard]] constexpr key_type        push_back(value_type&& newVal);
    [[nodiscard]] constexpr key_type        push_back(value_type const& newVal);
    [[nodiscard]] constexpr auto&           get(key_type key_value);
    [[nodiscard]] constexpr auto const&     get(key_type const key_value) const;
    
    
    constexpr void clear() noexcept;
    constexpr bool erase(key_type key) noexcept;
    [[nodiscard]] constexpr bool is_valid(key_type key) const noexcept;

    [[nodiscard]] constexpr iterator begin() noexcept;
    [[nodiscard]] constexpr iterator end() noexcept;
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept;
    [[nodiscard]] constexpr const_iterator cend() const noexcept;

private:
    [[nodiscard]] constexpr index_type allocate();
    constexpr void free(key_type key) noexcept;
    constexpr void freelist_init() noexcept;

    index_type                       size_{};
    index_type                       freelist_{};
    gen_type                         generation_{};
    std::array<key_type,   Capacity> indices_{};
    std::array<value_type, Capacity> data_{};
    std::array<index_type, Capacity> erase_{};
};

#include "Slotmap.tpp"

#endif
