#pragma once

#ifndef WAYPOINTCOMPONENT_H
#define WAYPOINTCOMPONENT_H

#include <memory>
#include <cstdint>

#include "../util/ObjectRigidType.hpp"
#include "../util/BasicUtils.hpp"

class btCollisionShape;
class btDefaultMotionState;
class btRigidBody;
class btDynamicsWorld;
class btTriangleMesh;


class WaypointComponent {
public:

    static constexpr int Capacity{100};

    std::unique_ptr<btCollisionShape> collisionShape;
    std::unique_ptr<btDefaultMotionState> motionState;
    std::unique_ptr<btRigidBody> rigidBody;

    btDynamicsWorld* dynamic_world_parent{nullptr};
    btDynamicsWorld* getParent() const;
    btDynamicsWorld* getParent();

    myVector3 tam;

    int id {-1};
    
    btRigidBody* getRigidBody();

    void initPhysics(btDynamicsWorld* dynamicsWorld, 
                        myVector3 pos, myVector3 size, int id, int idObj, float rotationZ, ObjectRigidType objectRigidType = ObjectRigidType::TYPE_UNKNOW,
                        float mass = 0.0f);

    void clearComponent();

};


#endif