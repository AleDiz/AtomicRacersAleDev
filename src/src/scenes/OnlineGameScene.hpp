#ifndef ONLINEGAMESCENE_H
#define ONLINEGAMESCENE_H

#include "Scene.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

#include "../util/OnlineStateRace.hpp"
#include "../util/EnumCircuits.hpp"
#include "../util/StateGame.hpp"

#include "raylib.h"
#include "rlgl.h"

// Escena del juego
class OnlineGameScene : public Scene {

private: 
    EntityFactory   Factory;
    EManager        EM;
    NetEngine       NE;
    EventManager    EVM;


    // Objetos del mundo físico
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration_;
    std::unique_ptr<btCollisionDispatcher> dispatcher_;
    std::unique_ptr<btDbvtBroadphase> broadphase_;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
    std::unique_ptr<btDynamicsWorld> world_;

    std::unique_ptr<btOverlapFilterCallback> filterCallback;

    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    const std::chrono::milliseconds tickRate{17}; // FPS 30


    // Variables circuito
    int vueltasMax      {};
    E* entityMusicRace      {};
    E* entityMusicIntro     {};
    E* sfxCountDown         {};

    float timer         {0.0f};
    float timer2        {0.0f};
    float timerPrimero  {0.0f};
    bool initTimer      {false};
    bool initTimer2     {false};
    bool timer2Expulse  {false};
    bool endRace        {false};
    bool changeSprites   {true};

    //Máscara de acciones
    myInput actionInputMask {};
    StateGame gameState {StateGame::CINEMATIC};

    std::array<E*, 5> HUD {};
    int stage {0};

    VehicleSystem               Vsystem;
    ObjectSystem                Osystem;
    RenderSystem                Rsystem;
    AISystem                    AIsystem;
    SoundSystem                 Ssystem;
    ClasificationSystem         CLsystem;
    CameraSystem                Csystem;
    InputSystem                 Isystem;
    eventSystem                 Esystem;
    MultiplayerSystem           Msystem;
    VehicleInputSystem          Visystem;
    NotAISystem                 NAIsystem;
    ParticleSystem              Psystem;
    

public:
    // Constructor
    OnlineGameScene(GameManager& gm) : Scene(gm) {}

    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;
    void clearHUD();

    void updateCinematic(float deltaTime);
    void updateCountdown(float deltaTime);
    void updateRace(float deltaTime);
    void updateEndRace(float deltaTime);

    std::chrono::steady_clock::time_point lastUpdatePlayer = std::chrono::steady_clock::now();
    
    void translateInput();
    void chargeLevel(LevelType lvlType, std::array<myVector3, 6>& posiciones, std::array<myVector3, 12>& animCinematic);
};


#endif 