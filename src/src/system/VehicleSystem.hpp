#pragma once

#ifndef VEHICLESYSTEM_H
#define VEHICLESYSTEM_H

class btDynamicsWorld; 
class btVector3;

#define MAX_SPEED 29.5
#define MAX_SPEED_BOOST 42.5
#define MAX_POWER 4000
#define ENGINE_POWER 3200
#define MIN_POWER 1600
#define DAMPING 0.5f

#define MAX_BOOST_DRIFT 1.5f
#define FACTOR_INCREASE_BOOST 0.01f


#define FACTOR_CETRIFUGAL_FORCE 15.0f
#define FACTOR_MINIMUM_SPEED_DRIFT 0.71f
#define FACTOR_INCREASE_SPEED_DRIFT 1.001f
#define FACTOR_REDUCTION_SPEED_MAX_LIMITED_SPEED 0.98f
#define FACTOR_REDUCTION_SPEED_MAX_BOOST 0.995f


#define FACTOR_INCREASE_STEERING_DRIFT 0.003f
#define FACTOR_INCREASE_STEERING_NORMAL 0.007f
#define FACTOR_RETURN_STEERING_ANYKEY 0.001f

#define MAX_STEERING_NORMAL 0.10f
#define ADJUST_STEERING_DRIFT_TO_MAX 0.1f
#define ADJUST_STEERING_DRIFT_TO_MIN 0.05f
#define MAX_STEERING_DRIFT (MAX_STEERING_NORMAL + ADJUST_STEERING_DRIFT_TO_MAX)
#define MIN_STEERING_DRIFT (MAX_STEERING_NORMAL - ADJUST_STEERING_DRIFT_TO_MIN)

#define FACTOR_WITHOUT_GAMEPAD 0.06f                                                    // Cuando sueltas en joystick no se queda en 0 sino +-0.06
#define STEERING_WITHOUT (FACTOR_WITHOUT_GAMEPAD * MAX_STEERING_NORMAL)

#define FACTOR_SPEED_TO_END_DRIFT 2.0f
#define MIN_SPEED_TO_END_DRIFT (MAX_SPEED / FACTOR_SPEED_TO_END_DRIFT)

#define INCREASE_SPEED_DRIFT_WHEN_LOWER_THAN_MAX 1.007f

#include "../util/UsingTypes.hpp"
#include "../util/BasicUtils.hpp"


enum class GroundType {
    NORMAL,
    SLOWDOWN,
    BOOST
};

struct EventManager;

class VehicleSystem {
public:
    void update(EManager& EM, EventManager& EVM, float deltaTime);
    void syncDataSaves(EManager& EM);
    void interpolate(EManager& EM, float iF);
    void savePhysicsPrevious(EManager& EM);
    void savePhysicsCurrent(EManager& EM);
    void remove(EManager& EM, btDynamicsWorld* dynamicsWorld);

private:
    static void interpolate_one_entity(E& e, float iF);
    static void savePhysicsPrevious_one_entity(E& e);
    static void savePhysicsCurrent_one_entity(E& e);
    static void update_one_entity(E& e, EventManager& EVM, float deltaTime);
    static void delete_one_entity(E& e, btDynamicsWorld* dynamicsWorld);
    static void updatePowerUpVehicle(E& e, EventManager& EVM, float deltaTime);   
    static void syncOnePlayer(E& e);
    static void testRayGround(VehicleComponent& vc, GroundType& groundT, int& idEntity);
    static void convertInputs(VehicleComponent& vc, myInput actionInput, float& throttle, float& brake, float deltaTime);
    static float calculateSpeedApproximatelyAfterImpulse(const float currentSpeed, const float mass, const btVector3 desiredImpulse);
    static void applyImpulseBoost(VehicleComponent& vc, float boost);
    
};

#endif