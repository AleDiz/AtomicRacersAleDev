#ifndef PREGAMESCENE_H
#define PREGAMESCENE_H

#include "Scene.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

// Escena de pre-juego
class PreGameScene : public Scene {
private:
    EntityFactory     Factory;
    EManager          EM;
    EventManager      EVM;
    std::array<E*, 32>   buttons;

    E* detalle;
    E* music;

    RenderSystem    Rsystem;
    ButtonSystem    Bsystem;
    SoundSystem     Ssystem;
    eventSystem     ESystem;

    
public:
    // Constructor
    PreGameScene(GameManager& gm) : Scene(gm) {}

    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;

    void loadPreGame(std::string path, EManager& EM, EntityFactory& EF);
};


#endif 