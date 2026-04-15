#pragma once

#ifndef RENDERSHAPECOMPONENT_H
#define RENDERSHAPECOMPONENT_H

#include <cstdint>

#include "../util/RenderUtils.hpp"


class RenderShapeComponent {
public:

    myShape shape{};

    static constexpr int Capacity{100};

    void initRender3dComponent(float H, float W, float L, myColor color);
    void clearComponent();

};

#endif