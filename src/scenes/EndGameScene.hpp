#ifndef ENDGAMESCENE_H
#define ENDGAMESCENE_H

#include "Scene.hpp"

// Escena de fin de juego
class EndGameScene : public Scene {
public:
    // Constructor
    EndGameScene(GameManager& gm) : Scene(gm) {}

    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;
};


#endif 