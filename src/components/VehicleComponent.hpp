#pragma once

#ifndef VEHICLECOMPONENT_H
#define VEHICLECOMPONENT_H

#include <memory>
#include <chrono>

#include <bullet/btBulletDynamicsCommon.h>          //WARNING ALE
#include <unordered_map>

#include "../util/PowerUpsTypes.hpp"
#include "../util/BasicUtils.hpp"


constexpr float VEHICLEPLAYERMASS       {500.0f};    //Masa del vehiculo del jugador
constexpr float ENGINEFORCEPLAYER       {5500.0f};   //Fuerza del motor del jugador
constexpr float BRAKEFORCE              {45.0f};     //Fuerza del motor del jugador
constexpr float MAXSPEEDPLAYER          {28.0};      //Fuerza del motor del jugador                             (NO)

constexpr float BOOSTMULT               {2.0f};      //Multiplicador de fuerza en el boost                    (NO)

constexpr float ANGLEMAXPLAYER          {0.15f};     //Ángulo de giro máximo sin derrapar
constexpr float ANGLEMINSIDESLIPPLAYER  {0.10f};     //Ángulo de giro minimo derrapando                        (NO)
constexpr float ANGLEMAXSIDESPLIPPLAYER {0.30f};     //Ángulo de giro máximo derrapando                        (NO)
constexpr float TURNSPEED               {45.0f};     //Velocidad de giro                                       (NO)

constexpr float DRAG                    {0.98f};     //Resistencia al aire                                     (NO)

constexpr float MAXSPEEDPLAYERBOOST     {53.0};      //Fuerza del motor del jugador con el powe up de boost     (NO)

constexpr int COOLDOWNPOWERUP           {50};        //Fuerza del motor del jugador con el powe up de boost
constexpr float COLLISION_COOLDOWN      { .5f};      // Tiempo mínimo entre colisiones


enum StateVehicle {
    NORMAL,
    DRIFTING, 
    REVERSE,
    BOOSTDRIFT,
    PENDINGBOOSTDRIFT
};

class VehicleComponent {
public:

    static constexpr int Capacity{6};


    std::unique_ptr<btBoxShape> chassisShape;                    //Necesito que el objeto que contiene el puntero que apunta a estos
    std::unique_ptr<btDefaultMotionState> vehicleMotionState;   //dos objetos no se destruya al salir de la función, de esta manera                                                        //se mantiene con vida mientras viva la instancia de la clase en el main

    std::unique_ptr<btRigidBody> m_carChassis;
    std::unique_ptr<btRaycastVehicle> m_vehicle;
    std::unique_ptr<btVehicleRaycaster> m_vehicleRaycaster;
    btRaycastVehicle::btVehicleTuning tuning;

    btDynamicsWorld* dynamic_world_parent{nullptr};
    btDynamicsWorld* getParent() const;
    btDynamicsWorld* getParent();

    // ------- Configuración Vehiculo --------
    float maxSpeed {MAXSPEEDPLAYER};
    float engineForce {ENGINEFORCEPLAYER};
    float brakeForce {BRAKEFORCE};
    float mass {VEHICLEPLAYERMASS};
    float damping {};
    float max_steering_normal{};
    float max_steering_drift{};
    float min_steering_drift{};
    float factor_increase_boost{};
    float wheel_radious{};
    float suspension_rest_length{};
    // --------------------------------------

    // Variables de control del vehiculo
    float throttle {0.0f};
    float brake {0.0f};
    bool drifting{false};
    float initDirectionDrift{0.0f};
    float driftBoost{0.0f};
    float storedBoostLastDrift {0.0f};
    btVector3 storedVelocity {};
    btVector3 storedVelocityAfterImpulse {};
    float maxSpeedInGame{MAXSPEEDPLAYER};

    float steering {0.0f};
    float steeringDrift {0.0f};
    bool boostWithDrift {false};

    StateVehicle stateVehicle = StateVehicle::NORMAL;
    StateVehicle previousStateDrift {StateVehicle::NORMAL};

    int directionStartDrift {};

    float boostCharge {0.0f};
    bool  gozBoost {false};
    // ---------------------------------

    float lastCollisionTime{ -1.0f }; // Tiempo de la última colisión

    PowerUps powerUp {PowerUps::ANYONE};
    PowerUps powerUpRender {PowerUps::ANYONE};
    PowerUps powerUpUsed {PowerUps::ANYONE};
    std::chrono::_V2::system_clock::time_point timerPowerUp;
    int tamPowerUp {0};

    int vueltas {0};
    int waypointSiguiente {0}; //Marca el id del waypoint al que tendria que ir
    int posicion {0};
    float tiempoCarrera {0.0f};

    const char* nombre {"vacio"};

    // Variables de interpolacion
    btTransform interpolatedTransform {};
    btTransform previousTransform {};
    btTransform currentTransform {};

    void setVehicleConfigurationValues(std::unordered_map<std::string, float> configuration);
    void initPhysics(btDynamicsWorld* dynamicsWorld, btVector3 positionInitial);

    void clearComponent();

    void setupWheels();

    myVector3 getWheelRightBackPos();
    myVector3 getWheelLeftBackPos();
};

#endif