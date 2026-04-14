#pragma once

#ifndef VEHICLEINPUTSYSTEM_H
#define VEHICLEINPUTSYSTEM_H


#include "../util/UsingTypes.hpp"
#include "../util/BasicUtils.hpp"


#define MAX_SPEED 29.5
#define MAX_SPEED_BOOST 42.5

#define MAX_STEERING_NORMAL 0.10f
#define FACTOR_INCREASE_STEERING_NORMAL 0.007f
#define FACTOR_MINIMUM_SPEED_DRIFT 0.71f
#define FACTOR_CETRIFUGAL_FORCE 15.0f
#define MAX_BOOST_DRIFT 1.5f

#define FACTOR_RETURN_STEERING_ANYKEY 0.001f

#define FACTOR_INCREASE_STEERING_DRIFT 0.003f
#define FACTOR_SPEED_TO_END_DRIFT 2.0f
#define MIN_SPEED_TO_END_DRIFT (MAX_SPEED / FACTOR_SPEED_TO_END_DRIFT)
#define FACTOR_WITHOUT_GAMEPAD 0.06f                                                    // Cuando sueltas en joystick no se queda en 0 sino +-0.06
#define STEERING_WITHOUT (FACTOR_WITHOUT_GAMEPAD * MAX_STEERING_NORMAL)

#define VELOCITY_LERP 1.35f


struct EventManager;

class VehicleInputSystem {
public:
    void update(EManager& EM, EventManager& EVM, float deltaTime);
   
private:
    static void update_one_entity(E& e, EventManager& EVM, float deltaTime);
    static void update_vehicle_with_inputs(VehicleComponent& vc, myInput actionInput, float deltaTime);
    static void process_events(E& e);
    static float Lerp(float a, float b, float t);
    
};

#endif