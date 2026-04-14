#pragma once

#include <cmath>

struct myVector2
{
    float x;
    float y;
};

struct myVector3
{
    float x;
    float y;
    float z;
    bool isZero(){
        return (x == 0 && y == 0 && z == 0);
    }
    void normalize() {
        float magnitude = std::sqrt(x * x + y * y + z * z);
        if (magnitude > 0) { // Avoid division by zero
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }
    myVector3 cross(const myVector3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    myVector3& operator=(const myVector3& other) {
        if (this != &other) { // Protección contra autoasignación
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    // Operador + para sumar dos myVector3
    myVector3 operator+(const myVector3& other) const {
        return {
            x + other.x,
            y + other.y,
            z + other.z
        };
    }

    // Operador += (opcional, pero útil)
    myVector3& operator+=(const myVector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    myVector3 normalized() const {
        myVector3 result = *this;
        result.normalize();
        return result;
    }
    
    myVector3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }
};

struct myVector4
{
    float x;
    float y;
    float z;
    float w;
};

struct myInput
{
    int inputMask;
    float R2;
    float L2;
    float RJ;
    float LJ;
};

struct myMatrix3x3
{
    myVector3 x;
    myVector3 y;
    myVector3 z;
};

struct myQuaternion
{
    float angle;
    myVector3 axis;

    myQuaternion& operator=(const myQuaternion& other) {
        if (this != &other) { // Protección contra autoasignación
            axis = other.axis;
            angle = other.angle;
        }
        return *this;
    }

    void fromAxisAngle(const myVector3& axis, float angleDegrees) {
        this->axis = axis;
        this->axis.normalize();
        this->angle = angleDegrees;
    }

    void fromAxisAngle2(const myVector3& axis, float angle) {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        this->axis.x = axis.x * sinHalf;
        this->axis.y = axis.y * sinHalf;
        this->axis.z = axis.z * sinHalf;
        this->angle = std::cos(halfAngle);
    }
};