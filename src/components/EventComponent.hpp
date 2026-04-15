#pragma once
#ifndef EVENT_COMPONENT_H
#define EVENT_COMPONENT_H

#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include "../util/EventTypes.hpp"
//struct EventData;

class EventComponent {
    private:
        
    public:
        static constexpr int Capacity{250};
        void clearComponent();

        std::vector<std::unique_ptr<EventData>> events;

        template <typename T> 
        const std::vector<T*> getEventDataType(){
            std::vector<T*> ev;
            for (auto& eventDat : events){
                if (auto* data = dynamic_cast<T*>(eventDat.get())) {
                    ev.push_back(data);
                }
            }
            return ev;
        }

        template <typename T> 
        void deleteEventsType() {
            events.erase(std::remove_if(events.begin(), events.end(), [](const std::unique_ptr<EventData>& eventDat) {
                return dynamic_cast<T*>(eventDat.get()) != nullptr;
            }), events.end());
        }
};

#endif // EVENT_COMPONENT_H