#pragma once

#ifndef OBJECTSYSTEM_H
#define OBJECTSYSTEM_H

#include "../util/UsingTypes.hpp"

class ObjectSystem {
public:
    void update(EManager& EM, float deltaTime);
    void interpolate(EManager& EM, float iF);
    void savePhysicsPrevious(EManager& EM);
    void savePhysicsCurrent(EManager& EM);

private:
    static void update_one_entity(E& e, float deltaTime);
    static void interpolate_one_entity(E& e, float iF);
    static void savePhysicsPrevious_one_entity(E& e);
    static void savePhysicsCurrent_one_entity(E& e);

};

#endif