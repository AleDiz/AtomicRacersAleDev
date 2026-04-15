#pragma once

#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <vector>

#include "BasicUtils.hpp"
#include <string>

struct myColor
{
    float r;
    float g;
    float b;
    float a;
};

struct myMesh
{
    myVector3 position;
    myQuaternion rotation;
    uint16_t id;
};

struct myParticle
{
    myVector3 position;
    myQuaternion rotation;
    uint16_t id;
};

struct myShape
{
    myVector3 position;
    myQuaternion rotation;
    float width;
    float height;
    float length;
    myColor color;
};

struct myTexture
{
    int width;
    int height;
    int mipmaps;
    int format;
    int filter;
    int wrap;
    int id;
};

struct myCamera
{
    myVector3 position;
    myVector3 target;
    myVector3 up;
    float fovy;
    int projection;
};

struct myLight
{
    myVector3 position;
    myColor color;
    float intensity;
};

struct myImage
{
    uint16_t id;
    myVector2 position;
    bool hover;
    myColor color;
};

struct myText
{
    std::string texto;
    myVector2 pos;
    int size;
    myColor color;
};

#endif