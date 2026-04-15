#ifndef TITLEMENUSCENE_H
#define TITLEMENUSCENE_H

#include "Scene.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

// Escena de opciones en menú principal
class TitleMenuScene : public Scene {
private:
    EntityFactory   Factory;
    EManager        EM;
    EventManager    EVM;
    std::array<E*, 200>   sprites;

    float timer     {0.0f};
    float timeToLoad {2.0f};
    bool initTimer  {false};


    RenderSystem    Rsystem;
    ButtonSystem    Bsystem;
    SoundSystem     Ssystem;
    eventSystem     ESystem;

    E* musicMenu;
public:
    // Constructor
    TitleMenuScene(GameManager& gm) : Scene(gm) {}


    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;

    void loadMenu(std::string path, EManager& EM, EntityFactory& EF);
};

#endif 