#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <functional>
#include <utility>
#include <typeindex>
#include "../util/EntityType.hpp" // Definición de EntityType
#include "../util/MetaFWD.hpp"  // Solo forward declaration
#include "../commons/ComponentCommons.h"  
#include "../util/UsingTypes.hpp"

template <typename CMPS, typename TAGS>
class Entity {
public:
    using ComponentKeys = Meta::ToComponentTuple_t<CMPS>;  // Tupla con las claves de componentes

    ComponentKeys componentKeys;
    EntityType tipo{EntityType::UNKNOWN};
    int componentMask = 0;
    EManager* parent_;
    int id;
    bool state{true};

    Entity(EManager& parent);

    EManager& getParent();
    const EManager& getParent() const;

    template <typename T>
    void setComponentKey(typename Slotmap<T, T::Capacity>::key_type key);

    template <typename T>
    std::optional<typename Slotmap<T, T::Capacity>::key_type> getComponentKey() const;

    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;
};

// Incluir la implementación
#include "Entity.tpp"

#endif // ENTITY_H
