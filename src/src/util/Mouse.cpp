// Mouse.cpp
#include "Mouse.h"

Mouse::Mouse() : position{GetMousePosition()} {}

void Mouse::Update() {
    position = GetMousePosition();
}

bool Mouse::IsButtonDown(int button) {
    return IsMouseButtonDown(button);
}

bool Mouse::IsButtonPressed(int button) {
    return IsMouseButtonPressed(button);
}

bool Mouse::IsButtonReleased(int button) {
    return IsMouseButtonReleased(button);
}

bool Mouse::IsButtonUp(int button) {
    return IsMouseButtonUp(button);
}

Vector2 Mouse::GetPosition() {
    return GetMousePosition();
}

float Mouse::GetX() {
    return GetMousePosition().x;
}

float Mouse::GetY() {
    return GetMousePosition().y;
}