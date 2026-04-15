#pragma once

#ifndef CLASIFICATIONSYSTEM_H
#define CLASIFICATIONSYSTEM_H

#include "../util/UsingTypes.hpp"
#include <vector>

class ClasificationSystem {
    public:
        void update(EManager& EM);
        void intercambiarPosicion(std::vector<int>& clasi, int pos1, int pos2);
        void intercambiarPosicion(VehicleComponent& e1, VehicleComponent& e2);
};

#endif

