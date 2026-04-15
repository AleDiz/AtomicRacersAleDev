#pragma once

#ifndef TNodo_H
#define TNodo_H

#include "TEntidad.hpp"
#include <vector>
#include <glm/vec3.hpp> 
#include <glm/mat4x4.hpp> 


class TNodo
{
    TEntidad* entity;
    std::vector <TNodo*> hijos;
    TNodo* padre;
    glm::vec3 traslacion;
    glm::vec3 rotacion;
    glm::vec3 escalado;
    glm::mat4 transformacion;

    int addHijo(TNodo* hijo);
    int removeHijo(TNodo* hijo);
    bool setEntity(TEntidad* entity);
    TEntidad* getEntity();
    TNodo* getPadre();
    void recorrer(glm::mat4 t);
    void setTraslacion(glm::vec3 traslacion);
    void setRotacion(glm::vec3 rotacion);
    void setEscalado(glm::vec3 escalado);
    void trasladar(glm::vec3 traslacion);
    void rotar(glm::vec3 rotacion);
    void escalar(glm::vec3 escalado);
    glm::vec3 getTraslacion();
    glm::vec3 getRotacion();
    glm::vec3 getEscalado();
    void setTransformacion(glm::mat4 transformacion);
    glm::mat4 getTransformacion();

    TNodo initArbol();
};

#endif