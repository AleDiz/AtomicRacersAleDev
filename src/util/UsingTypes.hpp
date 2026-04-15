#pragma once

#ifndef USINGTYPES_H
#define USINGTYPES_H

#include "Meta.hpp"

// Forward declarations de los componentes (evita incluirlos aquí)
class VehicleComponent;
class ObjectComponent;
class Render3dComponent;
class Render2dComponent;
class AIComponent;
class InputComponent;
class SoundComponent;
class WaypointComponent;
class ButtonComponent;
class CameraComponent;
class RenderShapeComponent;
class MultiplayerComponent;
class EventComponent;
class RenderTextComponent;
class NotAIComponent;
class ParticlesComponent;

// Lista de componentes
using GameComponents = Meta::TypeList<
    VehicleComponent, ObjectComponent, Render3dComponent, Render2dComponent,
    AIComponent, InputComponent, SoundComponent, WaypointComponent, 
    ButtonComponent, CameraComponent, RenderShapeComponent, MultiplayerComponent, 
    EventComponent, RenderTextComponent, NotAIComponent, ParticlesComponent>;

using OutGameComponents = Meta::TypeList<
    SoundComponent, ButtonComponent>;

using GameTags = Meta::TypeList<int>;

// Forward declarations de Entity y EntityManager
template <typename CMPS, typename TAGS>
class Entity;

template <typename CMPS, typename TAGS, template <typename, typename> class EntityT>
class EntityManager;

// Definir GameEntity y GameManager
using EManager = EntityManager<GameComponents, GameTags, Entity>;
using E        = Entity<GameComponents, GameTags>;

using OutGameManager = EntityManager<OutGameComponents, GameTags, Entity>;
using OutGameEntity = Entity<OutGameComponents, GameTags>;

#endif
