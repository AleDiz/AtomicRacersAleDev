#ifndef CHARGESCREENSCENE_H
#define CHARGESCREENSCENE_H

#include "Scene.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

// Escena de opciones en menú principal
class ChargeScreenScene : public Scene {
private:
    EntityFactory   Factory;
    EManager        EM;
    EventManager    EVM;

    RenderSystem    Rsystem;
    SoundSystem     Ssystem;
    eventSystem     ESystem;

    E* musicMenu;
public:
    // Constructor
    ChargeScreenScene(GameManager& gm) : Scene(gm) {}


    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;

    void loadMenu(std::string path, EManager& EM, EntityFactory& EF);
};

#endif 