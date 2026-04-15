#pragma once

#ifndef NOTAICOMPONENT_H
#define NOTAICOMPONENT_H

#include <cstdint>
#include <array>
#include <memory>

#include "../util/BasicUtils.hpp"

class NotAIComponent {

    public:
        static constexpr int Capacity{7}; 

        myVector3       posActual{};
        myVector3       pos1{};
        myVector3       pos2{};
        myQuaternion    rot{};

        bool hacia2     {true};

        void initNotAIComponent(myVector3 p1, myVector3 p2, myQuaternion r);

        void clearComponent();

};

#endif