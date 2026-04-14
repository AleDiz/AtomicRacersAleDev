#pragma once

#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

enum ComponentType{
    COMPONENT_VEHICLE  = 1 << 0,
    COMPONENT_OBJECT   = 1 << 1,
    COMPONENT_RENDER   = 1 << 2,
    COMPONENT_IA       = 1 << 3,
    COMPONENT_INPUT    = 1 << 4,
    COMPONENT_SOUND    = 1 << 5,
    COMPONENT_COLLIDER = 1 << 6,
    COMPONENT_WAYPOINT = 1 << 7,
    COMPONENT_MODELO   = 1 << 8,
    COMPONENT_BUTTON   = 1 << 9,
    COMPONENT_CAMERA   = 1 << 10,
};

#endif