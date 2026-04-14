#ifndef IN_SOUND_MENU_SCENE_H
#define IN_SOUND_MENU_SCENE_H

#include "Scene.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

// Escena de opciones de video
class InSoundMenuScene : public Scene {
private:
    EntityFactory   Factory;
    EManager        EM;
    EventManager    EVM;
    std::array<E*, 64>   sprites;

    RenderSystem    Rsystem;
    ButtonSystem    Bsystem;
    SoundSystem     Ssystem;
    eventSystem     ESystem;

    E* musicMenu;
public:
    // Constructor
    InSoundMenuScene(GameManager& gm) : Scene(gm) {}
    
    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;

    void loadMenu(std::string path, EManager& EM, EntityFactory& EF);
    void updateVolumeSprites(int startIdx, int endIdx, int volume);
};

#endif
