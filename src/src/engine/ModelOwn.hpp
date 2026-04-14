#pragma once

#ifndef MODELOWN_H
#define MODELOWN_H

#include <unordered_map>
#include <string>

#include "raylib.h"
#include "bullet/btBulletDynamicsCommon.h"

class ModelOwn
{

public:
    void loadModel3D(const char *fileName);
    void unloadModel();
    void drawModel(Vector3 position, float scale, Color color);
    void drawRoad();
    Model getModel();
private:
    Model modelo;
};

#endif