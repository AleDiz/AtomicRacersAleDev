#pragma once
#include <functional>
#include <typeindex>

#include "eventTypes.hpp"
#include "queue"

class EventDispatcher {
    private:
        // Mapa de funciones: Cada evento tiene una lista de callbacks
        std::unordered_map<std::type_index, std::vector<std::function<void(void*)>>> listeners;
        std::queue<std::function<void()>> eventQueue;

    public:
        // Registrar un evento genérico
        template<typename EventType>
        void suscribe(std::function<void(const EventType&)> callback) {
            std::type_index type = typeid(EventType);
            listeners[type].push_back([callback](void* event){
                callback(*static_cast<EventType*>(event));
            });
        }


        // Emitir un evento genérico
        template<typename EventType>
        void emit(const EventType& event){
            std::type_index type = typeid(EventType);

            if(listeners.find(type) != listeners.end()){
                for(auto& callback : listeners[type]) {
                    eventQueue.push([callback, event](){
                        callback((void*)&event);
                    });
                }
            }
        }

        void processEvents(){
            while(!eventQueue.empty()){
                eventQueue.front()();         // Ejecuta el primer callback en la cola
                eventQueue.pop();           // Eliminalo después de ejecutarlo
            }
        }

};