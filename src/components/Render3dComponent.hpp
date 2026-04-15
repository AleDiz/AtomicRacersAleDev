#pragma once

#ifndef Render3dComponent_H
#define Render3dComponent_H

#include <cstdint>


class Render3dComponent {
public:
    //CAMBIAR WARNING ALE
    uint16_t id{};

    static constexpr int Capacity{300};

    void initRender3dComponent(uint16_t givenId);
    void clearComponent();

};

#endif