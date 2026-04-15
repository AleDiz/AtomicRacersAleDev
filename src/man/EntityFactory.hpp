#pragma once

#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <vector>

#include <bullet/btBulletDynamicsCommon.h>
#include "../util/UsingTypes.hpp"

#include "../engine/NetEngine.hpp"

#include "../util/JsonManager.hpp"
#include "../engine/FmodSoundEngine.cpp"

#include "../util/ObjectRigidType.hpp"
#include "../util/PowerUpsTypes.hpp"

#include "../util/RenderUtils.hpp"
#include "../util/ParticleTypes.hpp"

class EntityFactory
{
public:

    static void creationRequestprocess(EManager& EM,  btDynamicsWorld* dynamicsWorld, ISound* soundEngine);

    static void creationOnlineRequestprocess(EManager& EM, NetEngine& NE, btDynamicsWorld* dynamicsWorld, ISound* soundEngine);
    
    static void vehicleCreations(const E& e, EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine);

    static E& CreateVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine, 
        myVector3 posicion, int pos, int id, const char* nombre);
    
    static E& CreateOwnOnlineVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine, 
        myVector3 posicion, int pos, int idModelo, const char* nombre, uint32_t nUniqueID);
    
    static E& CreateOnlineVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine, 
        myVector3 posicion, int pos, int idModelo, const char* nombre, uint32_t nUniqueID);
    

    static E& CreateIAVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, int posicion, const char* nombre,
        ISound* soundEngine, float arrivalRadius, float time2arrive, float visionDistance, float time2Update, myVector3 spawn, int id);
    
    static E& createEntityCamera(EManager& EM, std::array<myVector3, 12> animCamera);
    
    static E& createObjectNw(EManager& EM,  btDynamicsWorld* dynamicsWorld, myVector3 position, float rotationZ,
        const std::string& shade, int idObj, ISound* soundEngine,
        ObjectRigidType objectRigidType = ObjectRigidType::TYPE_UNKNOW, float mass = (0.0f), bool isbox=false); 

    static E& createObject(EManager& EM,  btDynamicsWorld* dynamicsWorld, std::unique_ptr<btCollisionShape> shape, btVector3 position, 
        btVector3 size, ISound* soundEngine, ObjectRigidType objectRigidType = ObjectRigidType::TYPE_UNKNOW, float mass = (0.0f), bool isbox=false);
    

    static E& createPowerUpObject(EManager& EM,  btDynamicsWorld* dynamicsWorld, myVector3 position, 
        const std::string& shade, int idObj, ISound* soundEngine, ObjectRigidType objectRigidType, PowerUps type, float mass = (0.0f));

    static E& createWaypoint(EManager& EM,  btDynamicsWorld* dynamicsWorld, myVector4 pos, 
        btVector3 size, int id, int idObj, ObjectRigidType objectRigidType = ObjectRigidType::TYPE_UNKNOW, float mass = (0.0f));

    static E& createEntityMusic(EManager& EM,  ISound* soundEngine, const std::string& pathEvent);

    static E& createEntitySound3D(EManager& EM,  ISound* soundEngine, const std::string& pathEvent, const myVector4 pos);

    static E& createEntityActivationSound(EManager& EM, btDynamicsWorld* dynamicsWorld,  ISound* soundEngine, const std::string& pathEvent, const myVector4 pos, const int id, const myVector4 pos3d);

    static E& createEntityOnlyRender(EManager& EM,  int idRender);

    static E& createBoostGround(EManager& EM,  btDynamicsWorld* dynamicsWorld, ISound* soundEngine, myVector4 pos, 
        btVector3 size, int idObj, ObjectRigidType objectRigidType = ObjectRigidType::TYPE_UNKNOW, float mass = (0.0f));

    static E& createButton(EManager& EM, ISound* soundEngine, int id, int idSprite, int posX, int posY, int sizeX, int sizeY, int alpha);

    static E& createSprite(EManager& EM, int id, int posX, int posY, int alpha);

    static E& createText(EManager& EM, std::string text, int posX, int posY, int size, myColor color);

    static E& createNotAIEntity(EManager& EM, ISound* soundEngine, myVector3 p1, myVector3 p2, myQuaternion ro, uint16_t id);

    static E& createMuestra(EManager& EM, uint16_t id);
    
    static E& createEntityFixedCamera(EManager& EM, myVector3 pos, myVector3 tar);

    static void particleCreations(EManager& EM, const ParticleTypes pT, int idEmitter);

    static E& createParticleStar(EManager& EM, const ParticleTypes pT, int idEmitter);
    static E& createParticleSpark(EManager& EM, const ParticleTypes pT, int idEmitter);
    static E& createParticleCloud(EManager& EM, const ParticleTypes pT, int idEmitter);
};


#endif