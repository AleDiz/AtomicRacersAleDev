#ifndef SCENE_H
#define SCENE_H

#pragma once

class GameManager;

// Sistema de gestión de escenas

// Clase base para las escenas
class Scene {
public:
    Scene(GameManager& gm) : gameManager(gm) {}
    virtual ~Scene() = default;
    
    virtual void init() = 0;            // Inicialización
    virtual void cleanup() = 0;         // Limpieza
    virtual void update(float deltaTime) = 0;          // Actualización    
    virtual void update_sim(float simDeltaTime) = 0;
    virtual void render() = 0;          // Renderizado

protected:
    GameManager& gameManager;
};

#endif
