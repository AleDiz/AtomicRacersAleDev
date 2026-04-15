#ifndef SHOPSCENE_H
#define SHOPSCENE_H

#include "Scene.hpp"

// Escena de la tienda
class ShopScene : public Scene {
private:
public:
    // Constructor
    ShopScene(GameManager& gm) : Scene(gm) {}


    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;
};

#endif 