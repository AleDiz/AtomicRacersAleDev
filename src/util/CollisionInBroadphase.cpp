#include "CollisionInBroadphase.hpp"
#include "../man/EntityManager.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EventManager.hpp"

// Constructor para inicializar el EntityManger
FilterCallback::FilterCallback(EManager* em, EventManager* evm) : EM(em), EVM(evm) {}

void FilterCallback::doObjectCollisionAction(E* e0, E* e1) const{
    auto& o = EM->getComponent<ObjectComponent>(e0->getComponentKey<ObjectComponent>().value());

    switch (o.type)
    {
    case PowerUps::SHELL:
    case PowerUps::SHELLX3:
        doCollisionActionShell(e0, e1);
        break;


    case PowerUps::ANYONE:
        break;

    case PowerUps::BOOST:
        break;

    case PowerUps::BOOSTX3:
        break;


    case PowerUps::BANANAX3:
    case PowerUps::BANANA:
        doCollisionActionBanana(e0,e1);
        break;
    }
}


void FilterCallback::doVehicleCollisionAction(E* e0, E* e1) const{
    auto& veh = EM->getComponent<VehicleComponent>(e0->getComponentKey<VehicleComponent>().value());

    if(EM->hasComponent<ObjectComponent>(*e1)){
        auto& obj = EM->getComponent<ObjectComponent>(e1->getComponentKey<ObjectComponent>().value());
        switch (obj.getRigidBody()->getUserIndex())
        {
        case ObjectRigidType::TYPE_BOX_POWER_UP:
        {
                if(obj.isCollide){
                    EM->addParticleRequest(e1->id, ParticleTypes::STAR);
                    
                    EVM->addEvent(EventTypes::TAKEBOX, std::make_unique<TakeBoxData>(e1->id, e1->id));
                    EVM->addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e1->id, 0));
                    obj.isCollide = false;
                    if(veh.powerUp == PowerUps::ANYONE && veh.powerUpUsed == PowerUps::ANYONE){
                        PowerUps pw = static_cast<PowerUps>(1+(rand() % numPowerUps));
                        veh.powerUp = pw;
                        veh.powerUpRender = pw;
                        veh.tamPowerUp = 1;

                        if(veh.powerUp == PowerUps::BANANAX3 || veh.powerUp == PowerUps::BOOSTX3 || veh.powerUp == PowerUps::SHELLX3 || veh.powerUp == PowerUps::COHETE){
                            veh.tamPowerUp = 3;

                            switch (veh.powerUp)
                            {
                            case PowerUps::BANANAX3:
                                veh.powerUp = PowerUps::BANANA;
                                break;
                            case PowerUps::BOOSTX3:
                                veh.powerUp = PowerUps::BOOST;
                                break;
                            case PowerUps::SHELLX3:
                                veh.powerUp = PowerUps::SHELL;
                                break;
                            case PowerUps::COHETE:
                                veh.powerUp = PowerUps::ANYONE;
                                break;
                            }
                        }
                    } 
                }
            break;
        }   
        case ObjectRigidType::TYPE_POWER_UP_RANDOM:
        {
            EM->getComponent<VehicleComponent>(e0->getComponentKey<VehicleComponent>().value()).m_carChassis->setUserIndex2(1);
            break;
        }
        case ObjectRigidType::TYPE_WALL:
        {
            //EM->getComponent<VehicleComponent>(e0->getComponentKey<VehicleComponent>().value()).m_carChassis->setUserIndex2(1);
            break;
        }
        case ObjectRigidType::TYPE_WALL_SPECIAL:
        {
            break;
        }
        case ObjectRigidType::TYPE_SOUND_ACTIVATION:
        {
            if(e0->tipo == EntityType::PLAYER)
                EVM->addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e1->id, 0));
            break;
        }
        } 
    }else if(EM->hasComponent<WaypointComponent>(*e1)){
        auto& way = EM->getComponent<WaypointComponent>(e1->getComponentKey<WaypointComponent>().value());
        auto i = EM->getEntitiesAux2().size();

        

        if((veh.waypointSiguiente == way.id) || (veh.waypointSiguiente <= way.id && veh.waypointSiguiente + 7 >= way.id)){

            if((veh.waypointSiguiente == i - 1) || (way.id == i - 1) ){
                veh.waypointSiguiente = 0;

                veh.vueltas += 1;
            }else{
                veh.waypointSiguiente = way.id + 1;
            }
            
        }

        if(EM->hasComponent<AIComponent>(*e0)){
            if(way.id < veh.waypointSiguiente){
                veh.waypointSiguiente = way.id + 1;
            }
        }
        
    }else if (EM->hasComponent<VehicleComponent>(*e1)) {

        auto& veh1 = EM->getComponent<VehicleComponent>(e0->getComponentKey<VehicleComponent>().value());
        auto& veh2 = EM->getComponent<VehicleComponent>(e1->getComponentKey<VehicleComponent>().value());

        // --- Cooldown para evitar impulsos repetidos ---
        static auto start = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        float currentTime = std::chrono::duration<float>(now - start).count();
        
        if (veh1.lastCollisionTime + COLLISION_COOLDOWN > currentTime ||
            veh2.lastCollisionTime + COLLISION_COOLDOWN > currentTime) {
            return;
        }
        
        veh1.lastCollisionTime = currentTime;
        veh2.lastCollisionTime = currentTime;

        // --- Obtener posiciones y orientaciones ---
        btVector3 pos1 = veh1.m_carChassis->getWorldTransform().getOrigin();
        btVector3 pos2 = veh2.m_carChassis->getWorldTransform().getOrigin();
        btVector3 forward1 = veh1.m_carChassis->getWorldTransform().getBasis().getColumn(2).normalized();
        btVector3 forward2 = veh2.m_carChassis->getWorldTransform().getBasis().getColumn(2).normalized();

        // --- Calcular vector de separación ---
        btVector3 separationDir = (pos1 - pos2).normalized();

        // --- Determinar lados de colisión ---
        // Usamos el producto punto entre forward y separationDir para decidir el lado
        float dot1 = forward1.dot(separationDir);
        float dot2 = forward2.dot(separationDir);

        // --- Aplicar impulsos laterales ---
        float impulseMagnitude = 10000.0f;
        btVector3 up(0, 1, 0);

        // Veh1: Impulso perpendicular a su forward
        btVector3 lateralDir1 = up.cross(forward1).normalized();
        // Si el punto de colisión está "delante" (dot1 > 0), empujamos hacia la derecha, sino izquierda
        btVector3 impulse1 = lateralDir1 * (dot1 > 0 ? 1 : -1) * impulseMagnitude;

        // Veh2: Impulso en dirección opuesta a veh1 para separarlos
        btVector3 lateralDir2 = up.cross(forward2).normalized();
        btVector3 impulse2 = lateralDir2 * (dot1 > 0 ? -1 : 1) * impulseMagnitude; // Opuesto a veh1

        veh1.m_carChassis->applyCentralImpulse(impulse1);
        veh2.m_carChassis->applyCentralImpulse(impulse2);

    }

}


void FilterCallback::doCollisionActionShell(E* e0, E* e1) const{        

    switch (e1->tipo)
    {
    case EntityType::OBJETOSMUNDO:
        {  
            auto& ec = EM->getComponent<ObjectComponent>(e1->getComponentKey<ObjectComponent>().value());
            
            if(ec.getRigidBody()->getUserIndex() != ObjectRigidType::TYPE_GROUND || ec.getRigidBody()->getUserIndex() != ObjectRigidType::TYPE_BOOST_GROUND || ec.getRigidBody()->getUserIndex() != ObjectRigidType::TYPE_GROUND_INFINITY){

                e0->state = false;
            }
            break;
        }
    case EntityType::UNKNOWN:
        {
            break;
        }
    case EntityType::PLAYER:
    case EntityType::PLAYERONLINE:
        {

            auto& ec = EM->getComponent<VehicleComponent>(e1->getComponentKey<VehicleComponent>().value());
            
            ec.m_carChassis->setLinearVelocity({0,0,0});

            e0->state = false;
            break;
        }

    case EntityType::IA:
        {

            auto& ec = EM->getComponent<VehicleComponent>(e1->getComponentKey<VehicleComponent>().value());
            ec.m_carChassis->setLinearVelocity({0,0,0});

            e0->state = false;
            break;
        }

    case EntityType::OBJETOCOLISIONABLE:
        {
            break;
        }

    case EntityType::WAYPOINT:
        {
            break;
        }

    case EntityType::SOUND:
        {
            break;
        }

    case EntityType::POWERUP:
        {
            break;
        }
    }
}

void FilterCallback::doCollisionActionBanana(E* e0, E* e1) const{        

    switch (e1->tipo)
    {
    case EntityType::OBJETOSMUNDO:
        {  
            break;
        }
    case EntityType::UNKNOWN:
        {
            break;
        }
    case EntityType::PLAYER:
    case EntityType::PLAYERONLINE:
        {

            auto& ec = EM->getComponent<VehicleComponent>(e1->getComponentKey<VehicleComponent>().value());
            ec.m_carChassis->setLinearVelocity({0,0,0});

            e0->state = false;
            break;
        }

    case EntityType::IA:
        {

            auto& ec = EM->getComponent<VehicleComponent>(e1->getComponentKey<VehicleComponent>().value());
            ec.m_carChassis->setLinearVelocity({0,0,0});

            e0->state = false;
            break;
        }

    case EntityType::OBJETOCOLISIONABLE:
        {
            break;
        }

    case EntityType::WAYPOINT:
        {
            break;
        }

    case EntityType::SOUND:
        {
            break;
        }

    case EntityType::POWERUP:
        {
            break;
        }
    }
}

bool FilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const{

    if(proxy0->m_clientObject && proxy1->m_clientObject){

        // bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask != 0);
        // collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask != 0);
        
        // if(collides){
            // Obtener los objetos de colisiÃ³n a partir de los proxies
            btCollisionObject *obj0 = static_cast<btCollisionObject*>(proxy0->m_clientObject);
            btCollisionObject *obj1 = static_cast<btCollisionObject*>(proxy1->m_clientObject);

            int id0 = obj0->getUserIndex3();
            int id1 = obj1->getUserIndex3();

            if(obj0 && obj1){
                
                auto* e0 = EM->getEntityById(id0);
                auto* e1 = EM->getEntityById(id1);


                if(e0 && e1){
                    

                    if(EM->hasComponent<ObjectComponent>(*e1)){ //Entramos si e1 es un objeto

                        auto& o = EM->getComponent<ObjectComponent>(e1->getComponentKey<ObjectComponent>().value());

                        if(o.rigidBody->getUserIndex() == ObjectRigidType::TYPE_POWER_UP_RANDOM){
                            doObjectCollisionAction(e1, e0);
                        }
                        if(o.rigidBody->getUserIndex() == ObjectRigidType::TYPE_WALL){
                            auto& entity1 = *e1;
                            auto& entity0 = *e0;
                        }
                    }
                    if(EM->hasComponent<ObjectComponent>(*e0)){ //Entramos si e1 es un objeto
                        auto& o = EM->getComponent<ObjectComponent>(e0->getComponentKey<ObjectComponent>().value());

        
                        if(o.rigidBody->getUserIndex() == ObjectRigidType::TYPE_POWER_UP_RANDOM){
                            doObjectCollisionAction(e0, e1);
                        }
                        if(o.rigidBody->getUserIndex() == ObjectRigidType::TYPE_WALL){
                            auto& entity0 = *e0;
                            auto& entity1 = *e1;
                        }
                    }

                    if(EM->hasComponent<VehicleComponent>(*e1)){ //Entramos si e1 es un objeto
                            doVehicleCollisionAction(e1, e0);
                    }

                    if(EM->hasComponent<VehicleComponent>(*e0)){ //Entramos si e1 es un objeto
                            doVehicleCollisionAction(e0, e1);
                    }
                }
            }

            
        // }

        }  

    return true;
}
