#pragma once

#ifndef MOUSE_H
#define MOUSE_H

#include "raylib.h"

class Mouse {
public:
    // Constructor
    Mouse();

    // Método para actualizar la posición del mouse
    void Update(); 
    // Métodos para verificar estados del mouse
    bool static IsButtonDown(int button);
    bool static IsButtonPressed(int button);
    bool static IsButtonReleased(int button);
    bool static IsButtonUp(int button);

    // Método para obtener la posición actual del mouse
    Vector2 static GetPosition();

    // Método para obtener las coordenadas X e Y individuales
    float static GetX();
    float static GetY();

private:
    // Almacenaremos la posición actual del mouse para un posible procesamiento adicional
    Vector2 position;
};

#endif