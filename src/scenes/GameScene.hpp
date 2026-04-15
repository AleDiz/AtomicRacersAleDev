#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Scene.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

#include "../util/EnumCircuits.hpp"
#include "../util/StateGame.hpp"


// Escena del juego
class GameScene : public Scene {

private: 
    EntityFactory   Factory;
    EManager        EM;
    EventManager    EVM;

    // Objetos del mundo físico
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration_;
    std::unique_ptr<btCollisionDispatcher> dispatcher_;
    std::unique_ptr<btDbvtBroadphase> broadphase_;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
    std::unique_ptr<btDynamicsWorld> world_;

    std::unique_ptr<btOverlapFilterCallback> filterCallback;

    // Variables circuito
    int vueltasMax;
    E* entityMusicRace      {};
    E* entityMusicIntro     {};
    E* sfxCountDown         {};

    float timer     {0.0f};
    bool initTimer  {false};
    bool firstRender {false};
    bool initializatedGame {false};

    int stage {0};

    //Máscara de acciones
    myInput actionInputMask {};
    StateGame gameState {StateGame::CINEMATIC};

    VehicleSystem               Vsystem;
    ObjectSystem                Osystem;
    RenderSystem                Rsystem;
    AISystem                    AIsystem;
    SoundSystem                 Ssystem;
    ClasificationSystem         CLsystem;
    CameraSystem                Csystem;
    InputSystem                 Isystem;
    eventSystem                 Esystem;
    VehicleInputSystem          Visystem;
    NotAISystem                 NAIsystem;
    ParticleSystem              Psystem;
    

public:
    // Constructor
    GameScene(GameManager& gm) : Scene(gm) {}

    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;

    void updateCinematic(float deltaTime);
    void updateCountdown(float deltaTime);
    void updateRace(float deltaTime);
    void updateEndRace(float deltaTime);

    void cleanup() override;
    void clearHUD();
    
    void translateInput();
    void chargeLevel(LevelType lvlType, std::array<myVector3, 6>& posiciones, std::array<myVector3, 12>& animCinematic);
    void initGame();
};


#endif 