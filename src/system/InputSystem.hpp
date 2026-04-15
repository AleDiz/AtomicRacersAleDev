#pragma once

#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "../util/UsingTypes.hpp"
#include "../util/BasicUtils.hpp"

class InputSystem {
public:
    void update(EManager& EM, myInput actionMask);
    void cleanInput(EManager& EM);
    void debugCars(EManager& EM);
    
private:
    static void update_one_entity(E& e, myInput actionMask);
    static void clean_one_entity(E& e);
    static void debug_one_entity(E& e);

};

#endif