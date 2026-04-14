#include "UtilityAI.hpp"
#include "../commons/ComponentCommons.h"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

// Implementación de Consideration
Consideration::Consideration(const std::string& name, float value) : name(name), value(value) {}
std::string Consideration::getName() const { return name; }
float Consideration::getValue() const { return value; }

// Implementación de Action
Action::Action(const std::string& name) : name(name), score(1.0f) {}

void Action::addConsideration(std::shared_ptr<Consideration> consideration) {
    considerations.push_back(consideration);
}

float Action::scoreAction(const E& v) {
    if(considerations.size() > 0){
            score = 1.0f;
        for (const auto& consideration : considerations) {
            float scoreConsideration = consideration->scoreConsideration(v);
            score *= scoreConsideration;

            if (score == 0) {
                return score;
            }
        }

        // Reescalado del score de la acción
        float originalScore = score;
        float modFactor = 1 - (1.0f / considerations.size());
        float makeupValue = (1 - originalScore) * modFactor;
        score = originalScore + (makeupValue * originalScore);

        return score;
    }else{
        return 0.0f;
    }
    
}

float Action::getScore() const { return score; }
std::string Action::getName() const { return name; }

//-------------------CONSIDERATIONS-----------------------------------

// Implementación de VelocityConsideration
VelocityConsideration::VelocityConsideration() : Consideration("Velocity", 0.0f) {}

float VelocityConsideration::scoreConsideration(const E& v) const {
    auto& vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    
    float actVel = vc.m_vehicle->getCurrentSpeedKmHour() * 1000 / 3600;

    float maxVel = vc.maxSpeed;

    float score = (actVel) / (maxVel);

    return std::clamp(score,0.0f,1.0f);
}

// Implementación de AccelerationConsideration (NO HACE NADA, FALTAN LOS DIFERENTES COCHES)
AccelerationConsideration::AccelerationConsideration() : Consideration("Acceleration", 0.0f) {}

float AccelerationConsideration::scoreConsideration(const E& v) const {
    [[maybe_unused]] auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    float myAc = vc.engineForce;
    float maxAc = 6000;
    float score = myAc/maxAc;

    return std::clamp(score,0.0f,1.0f);
}

// Implementación de Distance2WallConsideration
Distance2WallConsideration::Distance2WallConsideration() : Consideration("Distance Wall", 0.0f) {}

float Distance2WallConsideration::scoreConsideration(const E& v) const {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    float shortDistance = 0.0f;

    float distanceTR = aiComp.targetDistanceTOPRIGHT;
    float distanceTL = aiComp.targetDistanceTOPLEFT;

    if(aiComp.targetTypeTOPLEFT == 0 && aiComp.targetTypeTOPRIGHT == 0){
        shortDistance = std::min(distanceTL,distanceTR);
    }else if(aiComp.targetTypeTOPLEFT == 0){
        shortDistance = distanceTL;
    }else if(aiComp.targetTypeTOPRIGHT==0){
        shortDistance = distanceTR;
    }

    if (aiComp.targetTypeTOPLEFT != 0 && aiComp.targetTypeTOPRIGHT)
    {
        return 0.0f;
    }
    
    float maxDistance = aiComp.visionDistance;
    float score = std::log(maxDistance / (shortDistance + 1.0f)) + 0.1f;
    
    return std::clamp(score, 0.1f, 1.0f);
}

// Implementación de Distance2PowerupConsideration
Distance2PowerupConsideration::Distance2PowerupConsideration() : Consideration("Distance PowerUp", 0.0f) {}

float Distance2PowerupConsideration::scoreConsideration(const E& v) const {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());

    float shortDistance = 0.0f;

    float distanceTR = aiComp.targetDistanceTOPRIGHT;
    float distanceTL = aiComp.targetDistanceTOPLEFT;

    if(aiComp.targetTypeTOPLEFT == 2 && aiComp.targetTypeTOPRIGHT == 2){
        shortDistance = std::min(distanceTL,distanceTR);
    }else if(aiComp.targetTypeTOPLEFT == 2){
        shortDistance = distanceTL;
    }else if(aiComp.targetTypeTOPRIGHT==2){
        shortDistance = distanceTR;
    }else{
        return 0.0f;
    }

    float maxDistance = aiComp.visionDistance / 2;
    float score = std::log(maxDistance / (shortDistance + 1.0f)) + 0.1f;

    return std::clamp(score, 0.1f, 1.0f);
}

// Implementación de Distance2CarConsideration
Distance2CarConsideration::Distance2CarConsideration() : Consideration("Distance Car", 0.0f) {}

float Distance2CarConsideration::scoreConsideration(const E& v) const {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());

    float shortDistance = 0.0f;
    float distanceRR = aiComp.targetDistanceREARRIGHT;
    float distanceRL = aiComp.targetDistanceREARLEFT;

    if(aiComp.targetTypeREARLEFT == 1 && aiComp.targetTypeREARRIGHT == 1){
        shortDistance = std::min(distanceRL,distanceRR);
    }else if(aiComp.targetTypeREARLEFT == 1){
        shortDistance = distanceRL;
    }else if(aiComp.targetTypeREARRIGHT==1){
        shortDistance = distanceRR;
    }else{
        return 0.0f;
    }

    float maxDistance = aiComp.visionDistance;
    float score = std::log(maxDistance / (shortDistance + 1.0f)) + 0.1f;

    return std::clamp(score, 0.1f, 1.0f);
}

// Implementación de Distance2CarConsideration
Distance2CarAheadConsideration::Distance2CarAheadConsideration() : Consideration("Distance Car Ahead ", 0.0f) {}

float Distance2CarAheadConsideration::scoreConsideration(const E& v) const {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    float shortDistance = 0.0f;
    float distanceTR = aiComp.targetDistanceTOPRIGHT;
    float distanceTL = aiComp.targetDistanceTOPLEFT;

    if(aiComp.targetDistanceTOPLEFT == 1 && aiComp.targetDistanceTOPRIGHT == 1){
        shortDistance = std::min(distanceTR,distanceTL);
    }else if(aiComp.targetTypeTOPLEFT == 1){
        shortDistance = distanceTL;
    }else if(aiComp.targetTypeTOPRIGHT==1){
        shortDistance = distanceTR;
    }else{
        return 0.1f;
    }

    float maxDistance = aiComp.visionDistance;
    float score = std::log(maxDistance / (shortDistance + 1.0f)) + 0.1f;

    return std::clamp(score, 0.1f, 0.8f);
}

// Implementación de PositionConsideration
PositionConsideration::PositionConsideration() : Consideration("Position", 0.0f) {}

float PositionConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    int pos = vc.posicion;

    int maxPos = 6;

    float minScore = 0.1f;
    float t = (static_cast<float>(pos) - 1) / ((float)maxPos - 1);
    float score = minScore + (1.0f - minScore) * std::pow(t, 2.0f);
    return std::clamp(score, minScore, 1.0f);
}

// Implementación de LapConsideration
LapConsideration::LapConsideration() : Consideration("Lap", 0.0f) {}

float LapConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    int vueltas = vc.vueltas;

    int maxVueltas = 3;

    float score = static_cast<float>(vueltas) / (float)maxVueltas;
    return std::clamp(score, 0.0f, 1.0f);
}

// Implementación de PowerUpConsideration
PowerUpConsideration::PowerUpConsideration() : Consideration("PowerUp", 0.0f) {}

float PowerUpConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    auto powerUp = vc.powerUp;
    [[maybe_unused]] auto tamPowerUp = vc.tamPowerUp;

    if (powerUp == PowerUps::ANYONE) {
        return 0.0f;
    }

    return 1.0f;
}

// Implementación de InversePowerUpConsideration
InversePowerUpConsideration::InversePowerUpConsideration() : Consideration("Inversed PowerUp", 0.0f) {}

float InversePowerUpConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    auto powerUp = vc.powerUp;

    if (powerUp == PowerUps::ANYONE) {
        return 1.0f;
    }

    return 0.0f;
}

// Implementación de BoostConsideration
BoostConsideration::BoostConsideration() : Consideration("Boost", 0.0f) {}

float BoostConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    auto powerUp = vc.powerUp;
    auto tamPowerUp = vc.tamPowerUp;

    // Si tiene BOOST, escalamos el score (0.3 - 0.9)
    if (powerUp == PowerUps::BOOST) {
        return 1.0f;
    }

    return 0.1f;
}

// Implementación de ShellConsideration
ShellConsideration::ShellConsideration() : Consideration("Shell", 0.0f) {}

float ShellConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    auto powerUp = vc.powerUp;
    auto tamPowerUp = vc.tamPowerUp;

    // Si tiene BOOST, escalamos el score (0.3 - 0.9)
    if (powerUp == PowerUps::SHELL) {
        return 0.4f + 0.1f * ((float)tamPowerUp - 1); // Lineal: 0.5, 0.6, 0.7
    }

    return 0.0f;
}

// Implementación de BananaConsideration
BananaConsideration::BananaConsideration() : Consideration("Banana", 0.0f) {}

float BananaConsideration::scoreConsideration(const E& v) const {
    auto &vc = v.getParent().getComponent<VehicleComponent>(v.getComponentKey<VehicleComponent>().value());
    auto powerUp = vc.powerUp;
    auto tamPowerUp = vc.tamPowerUp;

    // Si tiene BOOST, escalamos el score (0.3 - 0.9)
    if (powerUp == PowerUps::BANANA) {
        return 0.4f + 0.1f * ((float)tamPowerUp - 1); // Lineal: 0.5, 0.6, 0.7
    }

    return 0.0f;
}

//Pathfollowing consideration
PathFollowingConsideration::PathFollowingConsideration() : Consideration("PathFollowing", 0.0f) {}

float PathFollowingConsideration::scoreConsideration([[maybe_unused]]const E& v) const {
    return 0.5f; 
}

//Pathfollowing consideration
GroundConsideration::GroundConsideration() : Consideration("Ground", 0.0f) {}

float GroundConsideration::scoreConsideration(const E& v) const {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    if(aiComp.targetTypeGroundL == 3 || aiComp.targetTypeGroundR == 3){
        if(aiComp.targetTypeGroundL == 3 && aiComp.targetTypeGroundR == 3){
            return 1.0f;
        }else{
            return 0.7f;
        }
    }else if (aiComp.targetTypeGroundL == 4 || aiComp.targetTypeGroundR == 4){
        return 1.0f;
    }
    else{
        return 0.0f;
    }
}

//--------------------ACTIONS--------------------------------------------

// Implementación de PathFollowingAction
PathFollowingAction::PathFollowingAction() : Action("PathFollowing") {}

void PathFollowingAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::PATHFOLLOWING;
}

// Implementación de OvertakeAction
OvertakeAction::OvertakeAction() : Action("Overtake") {}

void OvertakeAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::OVERTAKE;
}

// Implementación de GroundAction
GroundAction::GroundAction() : Action("Ground") {}

void GroundAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::GROUNDACTION;
}

// Implementación de AvoidAction
AvoidAction::AvoidAction() : Action("Avoid") {}

void AvoidAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::AVOID;
}

// Implementación de TakeObjectAction
TakeObjectAction::TakeObjectAction() : Action("TakeObject") {}

void TakeObjectAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::TAKEOBJECT;
}

// Implementación de UseObjectBoostAction
UseObjectBoostAction::UseObjectBoostAction() : Action("UseObjectBoost") {}

void UseObjectBoostAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::USEOBJECT;
}

// Implementación de UseObjectBananaAction
UseObjectBananaAction::UseObjectBananaAction() : Action("UseObjectBanana") {}

void UseObjectBananaAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::USEOBJECT;
}

// Implementación de UseObjectShellAction
UseObjectShellAction::UseObjectShellAction() : Action("UseObjectShell") {}

void UseObjectShellAction::execute(E& v) {
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());
    aiComp.behaviour = SB::USEOBJECT;
}
