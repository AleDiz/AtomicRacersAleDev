#pragma once

#ifndef COLLISIONINBROADPHASE_H
#define COLLISIONINBROADPHASE_H

#include <btBulletDynamicsCommon.h>

#include "../util/UsingTypes.hpp"

#include "../util/PowerUpsTypes.hpp"
#include "../util/ObjectRigidType.hpp"
#include "../events/eventDispatcher.cpp"
#include <iostream>
#include <chrono>

struct EventManager;

constexpr int numPowerUps = 6;

struct FilterCallback : public btOverlapFilterCallback {
    EManager* EM;
    EventManager* EVM;

    FilterCallback(EManager* em, EventManager* evm);

    void doObjectCollisionAction(E* e0, E* e1) const;
    void doVehicleCollisionAction(E* e0, E* e1) const;
    void doCollisionActionShell(E* e0, E* e1) const;
    void doCollisionActionBanana(E* e0, E* e1) const;

    virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override;
};

#endif // COLLISIONBROADPHASE_H
