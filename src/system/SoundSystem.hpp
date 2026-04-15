#pragma once

#include "../util/UsingTypes.hpp"

class ISound;
class EventDispatcher;

class SoundSystem {
public:
    void update(EManager& EM, ISound* soundEngine);
    void update_menus(EManager& EM, ISound* soundEngine);
    void changeVolumeAfterMenu(EManager& EM, ISound* soundEngine, int v1, int v2, int v3);
    void removeAllEventInstances(EManager& EM, ISound* soundEngine);

private:
    static void update_one_entity(E& e, ISound* soundEngine);
    static void update_one_volume(E& e, ISound* soundEngine, int v1, int v2, int v3);
    static void remove_one_instance_event(E& v, ISound* soundEngine);
    static void processEventsSound(E& e, SoundComponent& soundComponent);
    static void update_listenner_atributes(E& e, ISound* soundEngine);
};