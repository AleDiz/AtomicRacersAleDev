#pragma once

#ifndef ENTITYMANAGER_TPP
#define ENTITYMANAGER_TPP

#include "EntityManager.hpp"
#include "../util/Meta.hpp"

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
EntityManager<CMPS, TAGS, Entity>::EntityManager(std::size_t defaultsize) {
    entities_.reserve(defaultsize);
    entities_vehicles.reserve(8);
    entities_waypoints.reserve(50);
    entities_HUD.reserve(5);
    entityCreationQueue.reserve(16);
    particlesCreationQueue.reserve(16);
    entityOnlineCreationQueue.reserve(8);

    nextId = 0;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
typename EntityManager<CMPS, TAGS, Entity>::GameEntity& 
EntityManager<CMPS, TAGS, Entity>::CreateEntity() {
    auto& e = entities_.emplace_back(*this);
    e.id = nextId++;
    return e;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::addEntityToAux1() {
    entities_vehicles.push_back(entities_.size()-1);
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::addEntityToAux2() {
    entities_waypoints.push_back(entities_.size()-1);
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::addEntityToAux3() {
    entities_HUD.push_back(entities_.size()-1);
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
std::vector<int> const& EntityManager<CMPS, TAGS, Entity>::getEntitiesAux1() const{
    return entities_vehicles;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
std::vector<int> const& EntityManager<CMPS, TAGS, Entity>::getEntitiesAux2() const{
    return entities_waypoints;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
std::vector<int>& EntityManager<CMPS, TAGS, Entity>::getEntitiesAux3(){
    return entities_HUD;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
std::vector<typename EntityManager<CMPS, TAGS, Entity>::GameEntity> const& EntityManager<CMPS, TAGS, Entity>::getCreationQueue() const {
    return entityCreationQueue;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::clearCreationQueue() {
    entityCreationQueue.clear();
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::addEntityRequest(GameEntity& e) {
    entityCreationQueue.push_back(e);
}


template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
std::vector<std::tuple<int_least32_t, ParticleTypes>> const& EntityManager<CMPS, TAGS, Entity>::getParticlesCreationQueue() const {
    return particlesCreationQueue;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::clearParticlesCreationQueue() {
    particlesCreationQueue.clear();
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::addParticleRequest(int e, ParticleTypes pT) {
    particlesCreationQueue.emplace_back(e, pT);
}


template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
std::vector<sPlayerCreation> const& EntityManager<CMPS, TAGS, Entity>::getOnlineCreationQueue() const {
    return entityOnlineCreationQueue;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::clearOnlineCreationQueue() {
    entityOnlineCreationQueue.clear();
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::addEntityOnlineRequest(sPlayerCreation e) {
    entityOnlineCreationQueue.push_back(e);
}



template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
typename EntityManager<CMPS, TAGS, Entity>::GameEntity* 
EntityManager<CMPS, TAGS, Entity>::getEntityByType(EntityType tipo) {
    for (auto& e : entities_) {
        if (e.tipo == tipo) {
            return &e;
        }
    }
    return nullptr;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
typename EntityManager<CMPS, TAGS, Entity>::GameEntity* 
EntityManager<CMPS, TAGS, Entity>::getEntityById(int id) {
    for (auto& e : entities_) {
        if (e.id == id) {
            return &e;
        }
    }
    return nullptr;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
typename EntityManager<CMPS, TAGS, Entity>::GameEntity* 
EntityManager<CMPS, TAGS, Entity>::getEntityByPos(int id){
    return &entities_[id];
};

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
typename EntityManager<CMPS, TAGS, Entity>::GameEntity* 
EntityManager<CMPS, TAGS, Entity>::getEntityByName(int id){
    for (auto& e : entities_)
    {
        int name = getComponent<ButtonComponent>(e.template getComponentKey<ButtonComponent>().value()).getlabel();
        if(name == id){
            return &e;
        }
    }

    return nullptr;
};

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
typename EntityManager<CMPS, TAGS, Entity>::GameEntity* 
EntityManager<CMPS, TAGS, Entity>::getEntityByState(int id){
    for (auto& e : entities_)
    {
        int state = getComponent<ButtonComponent>(e.template getComponentKey<ButtonComponent>().value()).getState();
        if(state == id){
            return &e;
        }
    }

    return nullptr;
};

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename Func, typename... Args>
void EntityManager<CMPS, TAGS, Entity>::forAll(Func&& function, Args&&... args) {
    for (auto& e : entities_) {
        std::invoke(std::forward<Func>(function), e, std::forward<Args>(args)...);
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename Func, typename cmp, typename... Args>
void EntityManager<CMPS, TAGS, Entity>::forAllCondition(Func&& function, Args&&... args) {
    for (auto& e : entities_) {
        if (e.componentMask & cmps::template mask<cmp>()) {
            std::invoke(std::forward<Func>(function), e, std::forward<Args>(args)...);
        }
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename Func, typename... Args>
void EntityManager<CMPS, TAGS, Entity>::forAllAux1( Func&& function, Args&&... args){
    for (auto& e : entities_vehicles)
    {
        std::invoke(std::forward<Func>(function), entities_[e], std::forward<Args>(args)...);
    }
};

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename cmp>
Slotmap<cmp, cmp::Capacity>::key_type 
EntityManager<CMPS, TAGS, Entity>::addComponent(GameEntity& e) {
    constexpr size_t index = cmps::template id<cmp>();
    auto& slotmap = std::get<index>(components_);
    auto key = slotmap.push_back(cmp{});

    e.template setComponentKey<cmp>(key);
    e.componentMask |= cmps::template mask<cmp>();

    return key;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename cmp>
bool EntityManager<CMPS, TAGS, Entity>::hasComponent(GameEntity const& entity) const {
    return (entity.componentMask & cmps::template mask<cmp>()) && entity.template getComponentKey<cmp>().has_value();
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename cmp>
cmp& EntityManager<CMPS, TAGS, Entity>::getComponent(typename Slotmap<cmp, cmp::Capacity>::key_type key_value) {
    constexpr size_t index = cmps::template id<cmp>();
    auto& slotmap = std::get<index>(components_);
    return slotmap.get(key_value);
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename cmp>
const cmp& EntityManager<CMPS, TAGS, Entity>::getComponent(typename Slotmap<cmp, cmp::Capacity>::key_type key_value) const {
    constexpr size_t index = cmps::template id<cmp>();
    const auto& slotmap = std::get<index>(components_);
    return slotmap.get(key_value);
}

// Eliminación de entidades
template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::deleteEntities() {
    entities_vehicles.clear();
    entities_waypoints.clear();
    entities_HUD.clear();
    entities_.clear();
    nextId = 0;
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::deleteSlotmaps() {
    std::apply([](auto&... slotmaps) {
        (..., slotmaps.clear());
    }, components_);
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::comprobarState() {
    for (auto it = entities_.begin(); it != entities_.end(); ) {
        if (it->state == false) {
            auto idx = std::distance(entities_.begin(), it);

            std::apply([&](auto&... slotmaps) {
                (..., (
                    [&] {
                        using ComponentType = typename std::decay_t<decltype(slotmaps)>::value_type;
                        if (it->componentMask & cmps::template mask<ComponentType>()) {
                            if (auto key = it->template getComponentKey<ComponentType>()) {
                                auto& component = slotmaps.get(key.value());  
                                component.clearComponent();  
                                slotmaps.erase(key.value());  
                            }
                        }
                    }()
                ));
            }, components_);
            
            it = entities_.erase(it); // Borrar la entidad

            actualizarIndicesAux1(idx);
            actualizarIndicesAux2(idx);
            actualizarIndicesAux3(idx);

        } else {
            ++it;
        }
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::resetEntityManager() {
    // 1. Limpiar todos los componentes de todas las entidades (sin verificar state)
    for (auto& entity : entities_) {
        std::apply([&](auto&... slotmaps) {
            (..., (
                [&] {
                    using ComponentType = typename std::decay_t<decltype(slotmaps)>::value_type;
                    if (entity.componentMask & cmps::template mask<ComponentType>()) {
                        if (auto key = entity.template getComponentKey<ComponentType>()) {
                            auto& component = slotmaps.get(key.value());  
                            component.clearComponent();  
                            slotmaps.erase(key.value());  
                        }
                    }
                }()
            ));
        }, components_);
    }

    // 2. Borrar todos los slotmaps (elimina componentes restantes)
    deleteSlotmaps();

    // 3. Borrar todas las entidades y reiniciar el ID
    deleteEntities();
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::comprobarClearEntitiesAux1() {
    std::vector<int> deleted_indices;

    // 1. Encontramos todas las entidades que deben eliminarse y guardamos sus índices
    entities_vehicles.erase(std::remove_if(entities_vehicles.begin(), entities_vehicles.end(),
        [this, &deleted_indices](int n) { 
            if (!entities_[n].state) {
                deleted_indices.push_back(n);
                return true; // Marcar para eliminación
            }
            return false;
        }), entities_vehicles.end());

    // 2. Ordenamos los índices eliminados de menor a mayor
    std::sort(deleted_indices.begin(), deleted_indices.end());

    // 3. Ajustamos los índices restantes en entities_vehicles
    for (int& val : entities_vehicles) {
        int decrement = std::count_if(deleted_indices.begin(), deleted_indices.end(),
                                     [val](int removed) { return removed < val; });
        val -= decrement;
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::comprobarClearEntitiesAux2() {
    std::vector<int> deleted_indices;

    // 1. Encontramos todas las entidades que deben eliminarse y guardamos sus índices
    entities_waypoints.erase(std::remove_if(entities_waypoints.begin(), entities_waypoints.end(),
        [this, &deleted_indices](int n) { 
            if (!entities_[n].state) {
                deleted_indices.push_back(n);
                return true; // Marcar para eliminación
            }
            return false;
        }), entities_waypoints.end());

    // 2. Ordenamos los índices eliminados de menor a mayor
    std::sort(deleted_indices.begin(), deleted_indices.end());

    // 3. Ajustamos los índices restantes en entities_waypoints
    for (int& val : entities_waypoints) {
        int decrement = std::count_if(deleted_indices.begin(), deleted_indices.end(),
                                     [val](int removed) { return removed < val; });
        val -= decrement;
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::comprobarClearEntitiesAux3() {
    std::vector<int> deleted_indices;

    // 1. Encontramos todas las entidades que deben eliminarse y guardamos sus índices
    entities_HUD.erase(std::remove_if(entities_HUD.begin(), entities_HUD.end(),
        [this, &deleted_indices](int n) { 
            if (!entities_[n].state) {
                deleted_indices.push_back(n);
                return true; // Marcar para eliminación
            }
            return false;
        }), entities_HUD.end());

    // 2. Ordenamos los índices eliminados de menor a mayor
    std::sort(deleted_indices.begin(), deleted_indices.end());

    // 3. Ajustamos los índices restantes en entities_HUD
    for (int& val : entities_HUD) {
        int decrement = std::count_if(deleted_indices.begin(), deleted_indices.end(),
                                     [val](int removed) { return removed < val; });
        val -= decrement;
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::actualizarIndicesAux1(int idx){
    if (entities_vehicles.empty()) return;

    // Si la entidad borrada está antes del primer índice
    if (idx < entities_vehicles.front()) {
        for (auto& v : entities_vehicles) {
            --v;
        }
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::actualizarIndicesAux2(int idx){
    if (entities_waypoints.empty()) return;

    // Si la entidad borrada está antes del primer índice
    if (idx < entities_waypoints.front()) {
        for (auto& v : entities_waypoints) {
            --v;
        }
    }
}

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
void EntityManager<CMPS, TAGS, Entity>::actualizarIndicesAux3(int idx){
    if (entities_HUD.empty()) return;

    // Si la entidad borrada está antes del primer índice
    if (idx < entities_HUD.front()) {
        for (auto& v : entities_HUD) {
            --v;
        }
    }
}


template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
template <typename cmp>
void EntityManager<CMPS, TAGS, Entity>::delete_one_component(GameEntity& e) {
    if (!(e.componentMask & cmps::template mask<cmp>())) return;  // Si no tiene el componente, salir

    constexpr size_t index = cmps::template id<cmp>();  
    auto& slotmap = std::get<index>(components_);  // Obtener el Slotmap correcto

    if (auto key = std::get<index>(e.componentKeys)) {  // Obtener la clave desde la tupla
        auto& component = slotmap.get(key.value());  // Obtener el componente
        component.clearComponent();  // Llamar a la función antes de eliminarlo

        slotmap.erase(key.value());  // Ahora sí lo eliminamos
        std::get<index>(e.componentKeys).reset();  // Eliminar la clave de la tupla
        e.componentMask &= ~cmps::template mask<cmp>();  // Apagar el bit correspondiente
    }
};

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
int EntityManager<CMPS, TAGS, Entity>::getSizeEntities(){
    return entities_.size();
}

#endif


