#pragma once

#ifndef COMPONENTKEY_H
#define COMPONENTKEY_H

enum ComponentKey{
    COMPONENT_W        = 1 << 0,
    COMPONENT_S        = 1 << 1,
    COMPONENT_A        = 1 << 2,
    COMPONENT_D        = 1 << 3,
    COMPONENT_SF       = 1 << 4,
    COMPONENT_ENTER    = 1 << 5,
    COMPONENT_SP       = 1 << 6,
    COMPONENT_CTRL     = 1 << 7,
    COMPONENT_ESC      = 1 << 8,
};

#endif