#include "TNodo.hpp"
#include <memory>

/* LA IDEA ES TENER UN METODO QUE TE INICIE EL ARBOL BÁSICO DE UN MAPA, CON SUS LUCES Y DEMAS
TNodo TNodo::initArbol(circuits circuito)
{
    
    std::unique_ptr<TNodo> escena = std::make_unique<TNodo>(); 
    switch (circuito)
    {
    case circuits::MAPA_PRUEBA:
        
        break;
    case circuits::MAPA_JUEGOS:
        
        break; 
    case circuits::MAPA_ASEO:
        
        break;
    case circuits::MAPA_COCINA:
        
        break;
    return *escena;
    }
}
*/

int TNodo::addHijo(TNodo* hijo)
{
    hijos.push_back(hijo);
    return 0;
}

int TNodo::removeHijo(TNodo* hijo)
{
    for (int i = 0; i < hijos.size(); i++)
    {
        if (hijos[i] == hijo)
        {
            hijos.erase(hijos.begin() + i);
            return 0;
        }
    }
    return -1;
}

bool TNodo::setEntity(TEntidad* entity)
{
    this->entity = entity;
    return true;
}

TEntidad* TNodo::getEntity()
{
    return entity;
}

TNodo* TNodo::getPadre()
{
    return padre;
}

void TNodo::recorrer(glm::mat4 t)
{
    transformacion = t;
    for (int i = 0; i < hijos.size(); i++)
    {
        hijos[i]->recorrer(t);
    }
}

void TNodo::setTraslacion(glm::vec3 traslacion)
{
    this->traslacion = traslacion;
}

void TNodo::setRotacion(glm::vec3 rotacion)
{
    this->rotacion = rotacion;
}

void TNodo::setEscalado(glm::vec3 escalado)
{
    this->escalado = escalado;
}

void TNodo::trasladar(glm::vec3 traslacion)
{
    this->traslacion += traslacion;
}

void TNodo::rotar(glm::vec3 rotacion)
{
    this->rotacion += rotacion;
}

void TNodo::escalar(glm::vec3 escalado)
{
    this->escalado += escalado;
}

glm::vec3 TNodo::getTraslacion()
{
    return traslacion;
}

glm::vec3 TNodo::getRotacion()
{
    return rotacion;
}

glm::vec3 TNodo::getEscalado()
{
    return escalado;
}

void TNodo::setTransformacion(glm::mat4 transformacion)
{
    this->transformacion = transformacion;
}

glm::mat4 TNodo::getTransformacion()
{
    return transformacion;
}

