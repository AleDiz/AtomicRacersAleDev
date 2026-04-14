#pragma once

#ifndef NOTAISYSTEM_H
#define NOTAISYSTEM_H

#include "../util/UsingTypes.hpp"

class NotAISystem {
public:
    void update(EManager& EM, float deltaTime);

private:
    static void update_one_entity(E& e, float deltaTime);

};

#endif