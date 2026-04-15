#pragma once

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <algorithm>
#include <vector>
#include <functional>
#include <utility>
#include "../util/EntityType.hpp"
#include "../util/ParticleTypes.hpp"
#include "../util/MetaFWD.hpp" 
#include "../util/OnlineUtils.hpp"


template <typename TRAITS>
struct traits{
    using mask_type = Meta::IFT_t<(TRAITS::size() <= 8), 
        uint8_t,
        Meta::IFT_t<(TRAITS::size() <= 16),
            uint16_t,
            uint32_t>>;

    consteval static uint8_t size() noexcept {
        return TRAITS::size();
    };

    template<typename TRAIT>
    consteval static uint8_t id() noexcept {
        static_assert(TRAITS::template contains<TRAIT>());

        return TRAITS::template pos<TRAIT>();
    };

    template<typename TRAIT>
    consteval static mask_type mask() noexcept {
        return ( 1 << id<TRAIT>() );
    };

};

template <typename TAGLIST>
struct tags_traits : traits<TAGLIST> {

};

template <typename CMPS>
struct component_traits : traits<CMPS>{

};

template <typename CMPS, typename TAGS, template <typename, typename> class Entity>
class EntityManager {
public:
    using GameEntity = Entity<CMPS, TAGS>;
    using tags = tags_traits<TAGS>;
    using cmps = component_traits<CMPS>;

    explicit EntityManager(std::size_t defaultsize = 350);

    GameEntity& CreateEntity();
    void addEntityToAux1();
    void addEntityToAux2();
    void addEntityToAux3();
    void addEntityRequest(GameEntity& e);
    void addParticleRequest(int e, ParticleTypes pT);
    void addEntityOnlineRequest(sPlayerCreation e);
    
    std::vector<int> const& getEntitiesAux1() const;
    std::vector<int> const& getEntitiesAux2() const;
    std::vector<int>& getEntitiesAux3();
    
    std::vector<GameEntity> const& getCreationQueue() const;
    void clearCreationQueue();

    std::vector<std::tuple<int, ParticleTypes>> const& getParticlesCreationQueue() const;
    void clearParticlesCreationQueue();

    std::vector<sPlayerCreation> const& getOnlineCreationQueue() const;
    void clearOnlineCreationQueue();

    template <typename cmp>
    typename Slotmap<cmp, cmp::Capacity, std::uint32_t>::key_type addComponent(GameEntity& e);

    template <typename cmp>
    bool hasComponent(GameEntity const& entity) const;

    GameEntity* getEntityByType(EntityType tipo);
    GameEntity* getEntityById(int id);
    GameEntity* getEntityByPos(int id);
    GameEntity* getEntityByName(int id);
    GameEntity* getEntityByState(int id);

    template <typename Func, typename... Args>
    void forAll(Func&& function, Args&&... args);

    template <typename Func, typename cmp, typename... Args>
    void forAllCondition(Func&& function, Args&&... args);

    template <typename Func, typename... Args>
    void forAllAux1(Func&& function, Args&&... args);

    template <typename cmp>
    cmp& getComponent(typename Slotmap<cmp, cmp::Capacity>::key_type key_value);

    template <typename cmp>
    const cmp& getComponent(typename Slotmap<cmp, cmp::Capacity>::key_type key_value) const;

    void deleteEntities();
    void deleteSlotmaps();
    void comprobarState();
    void comprobarClearEntitiesAux1();
    void comprobarClearEntitiesAux2();
    void comprobarClearEntitiesAux3();

    void actualizarIndicesAux1(int idx);
    void actualizarIndicesAux2(int idx);
    void actualizarIndicesAux3(int idx);

    void resetEntityManager();

    template <typename cmp>
    void delete_one_component(GameEntity& e);

    int getSizeEntities();

private:
    std::vector<GameEntity> entities_{};
    std::vector<int> entities_vehicles{};
    std::vector<int> entities_waypoints{};
    std::vector<int> entities_HUD{};
    int nextId{0};
    std::vector<GameEntity> entityCreationQueue;
    std::vector<std::tuple<int, ParticleTypes>> particlesCreationQueue;
    std::vector<sPlayerCreation> entityOnlineCreationQueue{};

    Meta::ToSlotmap_t<CMPS> components_;   
};

#include "EntityManager.tpp"

#endif