#pragma once

#ifndef RenderTextComponent_H
#define RenderTextComponent_H

#include <cstdint>
#include "../util/RenderUtils.hpp"


class RenderTextComponent {
public:

    myText text{};

    static constexpr int Capacity{100};

    void initRenderTextComponent(std::string givenText, int givenX, int givenY, int givenSize, myColor givenColor);
    void clearComponent();

};

#endif