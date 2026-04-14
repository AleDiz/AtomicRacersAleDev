#pragma once
#include <vector>
#include <variant>
#include <memory>

#include "../util/EventTypes.hpp"

// Estructura general del evento
struct Event {
    EventTypes type;
    std::unique_ptr<EventData> data;
};


class EventManager {
    private:
        //std::unordered_map<EventTypes, std::vector<Event>> events;
        std::vector<Event> events;

    public:
        void addEvent(EventTypes type, std::unique_ptr<EventData> data){ events.push_back({type, std::move(data)}); };
        const std::vector<Event>& getEvents(){ return events; };
        
        template<typename T>
        T* getData(EventTypes type, const Event& e){
            // La comprobaciÃ³n del tipo ya viene hecha desde fuera
            return dynamic_cast<T*>(e.data.get());
        }
        
        void clear(){events.clear();};
};