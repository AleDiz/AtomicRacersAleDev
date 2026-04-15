#pragma once

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "../util/UsingTypes.hpp"

class ParticleSystem {
public:
    void update(EManager& EM, float deltaTime);

private:
    static void update_one_entity(E& e, float deltaTime);

};

#endif