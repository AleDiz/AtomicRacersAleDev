#include "WaypointComponent.hpp"
#include "bullet/btBulletDynamicsCommon.h"
#include "../util/JsonManager.hpp"

btRigidBody* WaypointComponent::getRigidBody(){
    return rigidBody.get();
}

void WaypointComponent::initPhysics(btDynamicsWorld* dynamicsWorld, 
                                    myVector3 pos, myVector3 size, int id, int idObj, float rotationZ, ObjectRigidType objectRigidType,
                                    float mass){

    dynamic_world_parent = dynamicsWorld;

    btVector3 position {pos.x, pos.y, pos.z};

    // ------ Con el id obtener el fileNameOBJ y obtener la bounding ------
    JSONManager jsonMan;
    jsonMan.loadJSON("assets/JSON/renderManager/grafico.json");
    std::array<std::string, 3> infoObj;
    jsonMan.getInfoID(idObj, infoObj);
    std::string pathOBJ = infoObj[0];


    dataObj data; 
    jsonMan.loadOBJData(pathOBJ, data);

    //Determinar el tamaño del objeto a partir de los vértices
    btVector3 minBox(FLT_MAX, FLT_MAX, FLT_MAX);
    btVector3 maxBox(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for(const auto& vertex : data.vertices_){
        minBox.setMin(btVector3(vertex.x, vertex.y, vertex.z));
        maxBox.setMax(btVector3(vertex.x, vertex.y, vertex.z));
    }

    btVector3 halfExtents = (maxBox - minBox) * 0.5;
    btVector3 center = (minBox + maxBox) * 0.5;
    center += position; // Aplicar la posición correctamente


    btQuaternion rotation(btVector3(0, 1, 0), rotationZ);
    btTransform transform(rotation, center);
    // ---------------------------------------------------------

    collisionShape = std::make_unique<btBoxShape>(halfExtents);
    motionState = std::make_unique<btDefaultMotionState>(transform);


    // Guardar el size
    tam = {size.x * 2, size.y * 2, size.z * 2};

    //Asignar la forma de colisión
    //collisionShape = std::move(shape);

    //Crear el motionState
    //btTransform initialTransformation;
    //initialTransformation.setIdentity();
    //initialTransformation.setOrigin(position);
    //motionState = std::make_unique<btDefaultMotionState>(initialTransformation);
    

    //Calcular la inercia si el objeto tiene masa
    btVector3 localInertia(0,0,0);
    if(mass != 0.0){
        collisionShape->calculateLocalInertia(mass, localInertia);
    }

    //Crear el cuerpo rígido
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), collisionShape.get(), localInertia);
    rigidBody = std::make_unique<btRigidBody>(rbInfo);


    rigidBody->setCollisionFlags(
        rigidBody->getCollisionFlags() | 
            btCollisionObject::CF_KINEMATIC_OBJECT | 
            btCollisionObject::CF_NO_CONTACT_RESPONSE
    );


    rigidBody->setActivationState(DISABLE_DEACTIVATION);

    rigidBody->setUserIndex(objectRigidType);
    rigidBody->setUserIndex2(0);                            // 0 la caja no esta activa 1 si esta activa
    rigidBody->setUserIndex3(id);
    dynamicsWorld->addRigidBody(rigidBody.get());
}

void WaypointComponent::clearComponent(){
    dynamic_world_parent->removeCollisionObject(rigidBody.get());
}

btDynamicsWorld* WaypointComponent::getParent() {
    return dynamic_world_parent;
}

btDynamicsWorld* WaypointComponent::getParent() const {
    return dynamic_world_parent;
}