#pragma once

#ifndef OBJECTCOMPONENT_H
#define OBJECTCOMPONENT_H

#include <memory>
#include <vector>
#include <cstdint>

#include "../util/ObjectRigidType.hpp"
#include "../util/PowerUpsTypes.hpp"
#include "../util/BasicUtils.hpp"
#include <bullet/btBulletDynamicsCommon.h>          //WARNING ALE


class ObjectComponent {
public:
    static constexpr int Capacity{250};

    std::unique_ptr<btCollisionShape> collisionShape;
    std::unique_ptr<btDefaultMotionState> motionState;
    std::unique_ptr<btRigidBody> rigidBody;
    std::unique_ptr<btTriangleMesh> triMesh_ = nullptr;

    btDynamicsWorld* dynamic_world_parent{nullptr};
    btDynamicsWorld* getParent() const;
    btDynamicsWorld* getParent();

    myVector3 tam;
    myVector3 posInc;
    btQuaternion rotationInit;

    bool isCollide {true};          // Puede ser cogida
    bool isColliding {false};       // Si esta en colision

    float timeRespawn   {5.0f};
    float temp          {0.0f};

    PowerUps type {ANYONE};
    
    btRigidBody* getRigidBody();

    btTransform interpolatedTransform {};
    btTransform previousTransform {};
    btTransform currentTransform {};

    void initPhysics(   btDynamicsWorld* dynamicsWorld, 
                        myVector3 position, myVector3 size, int id, const std::string& shade, int idObj, float rotationZ,
                        ObjectRigidType objectRigidType = ObjectRigidType::TYPE_UNKNOW, 
                        float mass = 0.0f );

    void clearComponent();
    

};


#endif