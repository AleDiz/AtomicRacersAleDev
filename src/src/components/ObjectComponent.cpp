#include "ObjectComponent.hpp"
#include "../util/JsonManager.hpp"

btRigidBody* ObjectComponent::getRigidBody(){
    return rigidBody.get();
}

void ObjectComponent::initPhysics(   btDynamicsWorld* dynamicsWorld,  
                    myVector3 position, myVector3 size, int id, const std::string& shade, int idObj, float rotationZ, 
                    ObjectRigidType objectRigidType, float mass ){

    posInc = position;
    // ------------- CREAR FORMA DE COLISIÓN -----------------
    if(shade != "NoAuto")       // Apartir de un obj
    {
        // Con el id obtener el fileNameOBJ y a partir de esta obtener la bounding o la mesh
        JSONManager jsonMan;
        jsonMan.loadJSON("assets/JSON/renderManager/grafico.json");
        std::array<std::string, 3> infoObj;
        jsonMan.getInfoID(idObj, infoObj);
        std::string pathOBJ = infoObj[0];

        // Obtener información de vertices y caras;
        dataObj data;
        jsonMan.loadOBJData(pathOBJ, data);

        if(shade == "mesh")     // Collision Malla de Triangulos
        {
            if (!triMesh_) { // Asegurar inicialización
                triMesh_ = std::make_unique<btTriangleMesh>();
            }
            for (const auto& face : data.faces_) {
                if (face.v1 >= data.vertices_.size() || face.v2 >= data.vertices_.size() || face.v3 >= data.vertices_.size()) {
                    std::cerr << "Error: Índice de vértice fuera de rango: " 
                            << face.v1 << ", " << face.v2 << ", " << face.v3 << std::endl;
                    continue;
                }

                const btVector3 v1(data.vertices_[face.v1].x, data.vertices_[face.v1].y, data.vertices_[face.v1].z);
                const btVector3 v2(data.vertices_[face.v2].x, data.vertices_[face.v2].y, data.vertices_[face.v2].z);
                const btVector3 v3(data.vertices_[face.v3].x, data.vertices_[face.v3].y, data.vertices_[face.v3].z);
                
                triMesh_->addTriangle(v1, v2, v3);
            }
            
            collisionShape = std::make_unique<btBvhTriangleMeshShape>(triMesh_.get(), true);
            motionState = std::make_unique<btDefaultMotionState>(
                btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x, position.z, position.y))
            );

            mass = 0;
            interpolatedTransform.setOrigin(btVector3(position.x, position.z, position.y));
            interpolatedTransform.setRotation(btQuaternion(0, 0, 0, 1));

        }
        else if(shade == "box")     // Collision Bounding Box
        {
            //Determinar el tamaño del objeto a partir de los vértices
            btVector3 minBox(FLT_MAX, FLT_MAX, FLT_MAX);
            btVector3 maxBox(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for(const auto& vertex : data.vertices_){
                minBox.setMin(btVector3(vertex.x, vertex.y, vertex.z));
                maxBox.setMax(btVector3(vertex.x, vertex.y, vertex.z));
            }

            btVector3 halfExtents = (maxBox - minBox) * 0.5;
            btVector3 center = (minBox + maxBox) * 0.5; // Centro de la caja

            if(! position.isZero()){
                center = { position.x,  position.z,  position.y};
            }

            collisionShape = std::make_unique<btBoxShape>(halfExtents);

            // Asegurar que la caja está en la posición correcta
            if(rotationZ != 0.0f){
                btQuaternion rotation(btVector3(0, 1, 0), rotationZ);
                btTransform transform(rotation, center);
                motionState = std::make_unique<btDefaultMotionState>(transform);
                interpolatedTransform.setOrigin(btVector3(position.x, position.z, position.y));
                interpolatedTransform.setRotation(btQuaternion(btVector3(0, 1, 0), rotationZ));
            }
            else{
                motionState = std::make_unique<btDefaultMotionState>(
                btTransform(btQuaternion(0, 0, 0, 1), center) 
                );
                interpolatedTransform.setOrigin(btVector3(position.x, position.z, position.y));
                interpolatedTransform.setRotation(btQuaternion(0, 0, 0, 1));
            } 
        }
    }
    else    // Formas Básicas 
    {
        if(objectRigidType == ObjectRigidType::TYPE_GROUND_INFINITY)
        {
            collisionShape = std::make_unique<btStaticPlaneShape>(btVector3(0, 1, 0), 0);
        }

        if(objectRigidType == ObjectRigidType::TYPE_SOUND_ACTIVATION)
        {
            collisionShape = std::make_unique<btBoxShape>(btVector3(1, 1, 1));
            
        }
        
        //Crear el motionState
        btTransform initialTransformation;
        initialTransformation.setIdentity();
        initialTransformation.setOrigin(btVector3(position.x,position.y,position.z));
        motionState = std::make_unique<btDefaultMotionState>(initialTransformation);

        interpolatedTransform.setOrigin(btVector3(position.x, position.z, position.y));
        interpolatedTransform.setRotation(btQuaternion(0, 0, 0, 1));
    }

    dynamic_world_parent = dynamicsWorld;
    
    // Guardar el size
    tam = {size.x * 2, size.y * 2, size.z * 2};

    //Calcular la inercia si el objeto tiene masa
    btVector3 localInertia(0,0,0);
    if(mass != 0.0){
        collisionShape->calculateLocalInertia(mass, localInertia);
    }

    //Crear el cuerpo rígido
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState.get(), collisionShape.get(), localInertia);
    rigidBody = std::make_unique<btRigidBody>(rbInfo);

    // ----------- CONFIGURAR RIGIDBODY ----------------
    if(objectRigidType == ObjectRigidType::TYPE_BOX_POWER_UP || objectRigidType == ObjectRigidType::TYPE_SOUND_ACTIVATION){
        // Configurar como cinemático y sin respuesta física
        rigidBody->setCollisionFlags(
            rigidBody->getCollisionFlags() | 
                btCollisionObject::CF_KINEMATIC_OBJECT | 
                btCollisionObject::CF_NO_CONTACT_RESPONSE
            );
    }
    else if(objectRigidType == ObjectRigidType::TYPE_WALL_SPECIAL)
    {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    }

    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    rigidBody->setUserIndex(objectRigidType);                   // Tipo de objeto
    if(objectRigidType == ObjectRigidType::TYPE_BOX_POWER_UP)
    {
        rigidBody->setUserIndex2(1);                            // 0|1 Caja Power Up // 0|1 Shell Destroy // 0|1 Cut Drift
    }
    else {
        rigidBody->setUserIndex2(0);                            // 0|1 Caja Power Up // 0|1 Shell Destroy // 0|1 Cut Drift
    }
    rigidBody->setUserIndex3(id);                               // ID del objeto                 

    // ----------------------------------------       

    dynamicsWorld->addRigidBody(rigidBody.get());
}

void ObjectComponent::clearComponent(){

    dynamic_world_parent->removeCollisionObject(rigidBody.get());
}

btDynamicsWorld* ObjectComponent::getParent() {
    return dynamic_world_parent;
}

btDynamicsWorld* ObjectComponent::getParent() const {
    return dynamic_world_parent;
}