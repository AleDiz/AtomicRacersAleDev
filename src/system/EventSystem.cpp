#include "EventSystem.hpp"

#include "../man/EventManager.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../util/EventTypes.hpp"

void eventSystem::update(EventManager& eventMan, EManager& EM) const{
    for(auto& event : eventMan.getEvents()){
        update_one_event(event, EM, eventMan);
    }
    eventMan.clear();
}

void eventSystem::update_one_event(const Event& event, EManager& EM, EventManager& eventMan) const{

    if(event.type == EventTypes::PLAYSOUND)
    {
        auto* data = eventMan.getData<PlaySoundData>(EventTypes::PLAYSOUND, event);
        auto* e = EM.getEntityById(data->idEntityToAdd);
        auto& ce = EM.getComponent<EventComponent>(e->getComponentKey<EventComponent>().value());
        ce.events.push_back(std::make_unique<PlaySoundData>(data->idEntityToAdd, data->indexSubSound));
    }
    else if(event.type == EventTypes::STOPSOUND)
    {
        auto* data = eventMan.getData<StopSoundData>(EventTypes::STOPSOUND, event);
        auto* e = EM.getEntityById(data->idEntityToAdd);
        auto& ce = EM.getComponent<EventComponent>(e->getComponentKey<EventComponent>().value());
        ce.events.push_back(std::make_unique<StopSoundData>(data->idEntityToAdd, data->indexSubSound));
    }
    else if(event.type == EventTypes::TAKEBOX)
    {
        auto* data = eventMan.getData<TakeBoxData>(EventTypes::TAKEBOX, event);
        auto* e = EM.getEntityById(data->idEntityToAdd);
        auto& ce = EM.getComponent<EventComponent>(e->getComponentKey<EventComponent>().value());
        ce.events.push_back(std::make_unique<TakeBoxData>(data->idEntityToAdd, data->idBox));
    }
    else if(event.type == EventTypes::COLLISIONWITHWALL)
    {
        auto* data = eventMan.getData<CollisionWithWallData>(EventTypes::COLLISIONWITHWALL, event);
        auto* e = EM.getEntityById(data->idEntityToAdd);
        auto& ce = EM.getComponent<EventComponent>(e->getComponentKey<EventComponent>().value());
        ce.events.push_back(std::make_unique<CollisionWithWallData>(data->idEntityToAdd, data->idEntity1, data->idEntity2));
    }
}