#pragma once

#ifndef Render2dComponent_H
#define Render2dComponent_H

#include <cstdint>
#include "../util/RenderUtils.hpp"


class Render2dComponent {
public:

    uint16_t id{};
    myVector2 position{};
    myColor color{};

    static constexpr int Capacity{300};

    void initRender2dComponent(int givenId, int givenX, int givenY, int givenAlpha);
    void clearComponent();

};

#endif