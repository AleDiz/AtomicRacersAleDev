#include "Slotmap.hpp"

// Constructor
template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr Slotmap<DataType, Capacity, INDEXT>::Slotmap() {
    clear();
}

// Métodos públicos
template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr std::size_t Slotmap<DataType, Capacity, INDEXT>::size() const noexcept {
    return size_;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr std::size_t Slotmap<DataType, Capacity, INDEXT>::capacity() const noexcept {
    return Capacity;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr typename Slotmap<DataType, Capacity, INDEXT>::key_type 
Slotmap<DataType, Capacity, INDEXT>::push_back(value_type&& newVal) {
    auto reserveid = allocate();
    auto& slot = indices_[reserveid];
    
    data_[slot.id]  = std::move(newVal);
    erase_[slot.id] = reserveid;

    auto key { slot };
    key.id = reserveid;

    return key;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr typename Slotmap<DataType, Capacity, INDEXT>::key_type 
Slotmap<DataType, Capacity, INDEXT>::push_back(value_type const& newVal) {
    return push_back(value_type{newVal});
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr auto& Slotmap<DataType, Capacity, INDEXT>::get(key_type key_value) {
    assert(is_valid(key_value));
    return data_[indices_[key_value.id].id];
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr const auto& Slotmap<DataType, Capacity, INDEXT>::get(key_type const key_value) const {
    assert(is_valid(key_value));
    return data_[indices_[key_value.id].id];
}


template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr void Slotmap<DataType, Capacity, INDEXT>::clear() noexcept {
    size_ = 0;
    freelist_ = 0;
    generation_ = 0;
    for (auto& index : indices_) { index = {0, 0}; }
    for (auto& erase_index : erase_) { erase_index = 0; }
    for (auto& datum : data_) { datum = {}; }

    freelist_init();
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr bool Slotmap<DataType, Capacity, INDEXT>::erase(key_type key) noexcept {
    if (!is_valid(key)) return false;
    free(key);
    return true;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr bool Slotmap<DataType, Capacity, INDEXT>::is_valid(key_type key) const noexcept {
    return key.id < Capacity && indices_[key.id].gen == key.gen;
}

// Iteradores
template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr Slotmap<DataType, Capacity, INDEXT>::iterator
Slotmap<DataType, Capacity, INDEXT>::begin() noexcept {
    return data_.begin();
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr Slotmap<DataType, Capacity, INDEXT>::iterator
Slotmap<DataType, Capacity, INDEXT>::end() noexcept {
    return data_.begin() + size_;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr Slotmap<DataType, Capacity, INDEXT>::const_iterator
Slotmap<DataType, Capacity, INDEXT>::cbegin() const noexcept {
    return data_.cbegin();
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr Slotmap<DataType, Capacity, INDEXT>::const_iterator
Slotmap<DataType, Capacity, INDEXT>::cend() const noexcept {
    return data_.cbegin() + size_;
}

// Métodos privados
template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr typename Slotmap<DataType, Capacity, INDEXT>::index_type 
Slotmap<DataType, Capacity, INDEXT>::allocate() {
    if (size_ >= Capacity) throw std::runtime_error("No hay espacio libre en el slot");
    assert(freelist_ < Capacity);
    
    auto slotid = freelist_;
    freelist_ = indices_[slotid].id;

    auto& slot = indices_[slotid];
    slot.id = size_;
    slot.gen = generation_;

    ++size_;
    ++generation_;

    return slotid;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr void Slotmap<DataType, Capacity, INDEXT>::free(key_type key) noexcept {
    assert(is_valid(key));

    auto& slot = indices_[key.id];
    auto dataid = slot.id;

    slot.id = freelist_;
    slot.gen = generation_;
    freelist_ = key.id;

    if (dataid != size_ - 1) {
        data_[dataid]  = std::move(data_[size_ - 1]);
        erase_[dataid] = erase_[size_ -1];
        indices_[erase_[dataid]].id = dataid;
    }

    --size_;
    ++generation_;
}

template <typename DataType, std::size_t Capacity, typename INDEXT>
constexpr void Slotmap<DataType, Capacity, INDEXT>::freelist_init() noexcept {
    for (index_type i{}; i < indices_.size(); i++) {
        indices_[i].id = i + 1;
    }
    freelist_ = 0;
}
