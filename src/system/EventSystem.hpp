#pragma once

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "../util/UsingTypes.hpp"

struct EventManager;
struct Event;

class eventSystem {
    public:
        void update(EventManager& eventMan, EManager& EM) const;
    private:
        void update_one_event(const Event& e, EManager& EM, EventManager& eventMan) const;
};

#endif
