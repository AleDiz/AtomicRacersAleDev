#include "VehicleSystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../util/EnumsActions.hpp"

#include "../man/EventManager.hpp"

#include <GLFW/glfw3.h>
#include <chrono>

void VehicleSystem::update(EManager& EM, EventManager& EVM, float deltaTime){
    EM.forAllAux1<void(*)(E&, EventManager&, float)>(update_one_entity, EVM, deltaTime);
};

void VehicleSystem::syncDataSaves(EManager& EM){
    EM.forAllAux1<void(*)(E&)>(syncOnePlayer);
};

void VehicleSystem::interpolate(EManager& EM, float iF){
    EM.forAllAux1<void(*)(E&, float)>(interpolate_one_entity, iF);
};

void VehicleSystem::savePhysicsPrevious(EManager& EM){
    EM.forAllAux1<void(*)(E&)>(savePhysicsPrevious_one_entity);
};

void VehicleSystem::savePhysicsCurrent(EManager& EM){
    EM.forAllAux1<void(*)(E&)>(savePhysicsCurrent_one_entity);
};

void VehicleSystem::remove(EManager& EM, btDynamicsWorld* dynamicsWorld){
    EM.forAllCondition<void(*)(E&, btDynamicsWorld*), VehicleComponent>(delete_one_entity, dynamicsWorld);
};


void VehicleSystem::interpolate_one_entity(E& e, float iF){

    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());

    iF = std::clamp(iF, 0.0f, 1.0f);

    vc.interpolatedTransform.setOrigin(
        vc.previousTransform.getOrigin().lerp(
            vc.currentTransform.getOrigin(),
            iF
        )
    );

    vc.interpolatedTransform.setRotation(
        vc.previousTransform.getRotation().slerp(
            vc.currentTransform.getRotation(),
            iF
        )
    );
};

void VehicleSystem::savePhysicsPrevious_one_entity(E& e){

    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());

    vc.previousTransform = vc.currentTransform;

};

void VehicleSystem::savePhysicsCurrent_one_entity(E& e){

    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());

    vc.currentTransform = vc.m_carChassis->getWorldTransform();

};

//Al marcarlo como static hacemos que esta funcion no dependa de si se crea o no el hysic sistem, sino que se crea siempre que empieza el programa
//Lo hacemos porque, si no lo ponemos, como en la funcion que hemos declarado arriba del forall
//hemos obligado a que solo reciba un parámetro y la funcion requeria 2 (una instancia this de Physicsystem), daba errores de que pasabamos 
//mas parámetros de los pedidos
//Es como una funcion libre pero que esta dentro del espacio de nombres de PhysicSystem

void VehicleSystem::update_one_entity(E& e, EventManager& EVM, float deltaTime){
    
    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    vc.m_carChassis->clearForces();

    if(vc.m_carChassis->getUserIndex2() == 1){
        vc.m_carChassis->setUserIndex2(0);
        vc.drifting = false;
        vc.stateVehicle = StateVehicle::NORMAL;
        EVM.addEvent(EventTypes::STOPSOUND, std::make_unique<StopSoundData>(e.id, 1));
        EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 4));
    }

    if(vc.gozBoost)
    {
        float boostStart = 4.5f;
        btVector3 forward = vc.m_carChassis->getWorldTransform().getBasis() * btVector3(0, 0, 1);
        btVector3 desiredImpulse = forward * ((boostStart * boostStart) * vc.engineForce);
        vc.m_carChassis->applyCentralImpulse(desiredImpulse);

        // Si la nueva velocidad supera el máximo permitido, se limita
        float maxAllowedSpeed = 40.5f;
        float realNewSpeed = vc.m_carChassis->getLinearVelocity().length();
        if (realNewSpeed > maxAllowedSpeed) 
        {
            btVector3 limitedVelocity = vc.m_carChassis->getLinearVelocity().normalized() * maxAllowedSpeed;
            vc.m_carChassis->setLinearVelocity(limitedVelocity);
        }

        // vc.stateVehicle = StateVehicle::BOOSTDRIFT;
        // vc.driftBoost = 0.0f;
        // vc.gozBoost = false;
    }

    // ····················· GESTIÓN DE LOS INPUTS ·····························
    myInput inputMask {};
    if(e.getParent().hasComponent<InputComponent>(e))
    {
        inputMask = e.getParent().getComponent<InputComponent>(e.getComponentKey<InputComponent>().value()).actionInput;    
    }
    else if(e.getParent().hasComponent<AIComponent>(e))
    {
        inputMask = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value()).actionInput;
    }

    float throttle = vc.throttle;
    float brake = vc.brake;
    // ·····················································································

    // --------- DETECTAR TIPO SUELO (RAYO) ------------------
    GroundType groundT { };
    int idGroundEntity {-1};
    testRayGround(vc, groundT, idGroundEntity);
    // -------------------------------------------------------

    if(groundT == GroundType::BOOST){
        if(vc.m_carChassis->getLinearVelocity().length() < MAX_SPEED_BOOST){
            //vc.driftBoost = MAX_BOOST_DRIFT;
            
            auto pos = e.getParent().getComponent<ObjectComponent>(e.getParent().getEntityById(idGroundEntity)->getComponentKey<ObjectComponent>().value()).rigidBody->getWorldTransform().getOrigin();
    
            EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(idGroundEntity, 0));
        }
    }
    

    // ····················· AJUSTE DEL DAMPING ·····························
    if ((throttle == 0 && brake == 0) || (groundT == GroundType::SLOWDOWN))
        vc.m_carChassis->setDamping(vc.damping, vc.damping);
    else if(groundT != GroundType::SLOWDOWN)
        vc.m_carChassis->setDamping(0.0f, 0.0f); // Menor resistencia cuando aceleras
    // ·····················································································


    // ····················· GESTIÓN DEl DRIFT ·····························
    // Control de Inicio de Drift
    // Control de final de drift

    // ·····················································································

    // ····················· GESTIÓN DEl MOVIMIENTO ·····························
    
    if (vc.drifting)
    {
        // ----------- APLICAR ANGULO DE GIRO ------------
        vc.m_vehicle->setSteeringValue(vc.steeringDrift, 0);
        vc.m_vehicle->setSteeringValue(vc.steeringDrift, 1);
        // ------------------------------------------------
        
        // ----------- AJUSTAR VELOCIDAD ALMACENADA ------------
        btVector3 currentDirection = vc.m_carChassis->getLinearVelocity().normalized();

        if(groundT == GroundType::BOOST)
        {
            vc.storedVelocity = currentDirection * MAX_SPEED_BOOST;
            vc.storedVelocityAfterImpulse = vc.storedVelocity;
            vc.boostWithDrift = true;
            vc.stateVehicle = StateVehicle::BOOSTDRIFT;
        }
        if(groundT == GroundType::SLOWDOWN)
        {
            float dampingPerSecond = powf( 0.98f, 60.0f );  // 0.985 aplicado 60 veces
            vc.storedVelocity *= powf( 0.4f, deltaTime );  // simDeltaTime = 1/15
        }
        // -----------------------------------------------------

        // ----------- APLICAR VELOCIDAD ALMACENADA ------------
        float factorIncreaseSpeed = 0.15f;
        float maxSpeedMultiplier = 1.0f + (((vc.posicion - 1)/6.0f) * factorIncreaseSpeed);
        float maxSpeed = maxSpeedMultiplier * vc.maxSpeed;

        

        if(vc.storedVelocity.length() < maxSpeed - 0.02)             // Aumentar velocidad si es menor a la máxima
        {
            float driftBoost = pow( INCREASE_SPEED_DRIFT_WHEN_LOWER_THAN_MAX, deltaTime * 60.0f );          // Equivalente a la de abajo: pow(base, x * y) == pow(pow(base, x), y)
            vc.storedVelocity *= driftBoost;
            vc.m_carChassis->setLinearVelocity( currentDirection * vc.storedVelocity.length() );
        }
        else if(vc.storedVelocity.length() > maxSpeed  + 0.02)       // Reducir velocidad si es mayor a la máxima
        {
            if(vc.previousStateDrift == StateVehicle::BOOSTDRIFT || vc.boostWithDrift)
            {
                vc.storedVelocity *= powf( FACTOR_REDUCTION_SPEED_MAX_BOOST, deltaTime * 60.0f );
                vc.m_carChassis->setLinearVelocity( currentDirection * vc.storedVelocity.length() );
            }
            else
            {
                // float dampingPerSecond = powf( 0.985f, 60.0f );               // 0.985 aplicado 60 veces
                // vc.storedVelocity *= powf( 0.4f, deltaTime );
                vc.m_carChassis->setLinearVelocity( currentDirection * maxSpeed );

                if(vc.stateVehicle == StateVehicle::BOOSTDRIFT)
                {
                    vc.stateVehicle = StateVehicle::NORMAL;
                }
            }                 
        }
        else                                                           // Es igual a la máxima
        {

            vc.storedVelocity = currentDirection * maxSpeed;
            vc.m_carChassis->setLinearVelocity( currentDirection * vc.storedVelocity.length() );
            if(vc.stateVehicle)
            {
                vc.stateVehicle = StateVehicle::NORMAL;
            }
        }
        
        
        
        // ----------- APLICAR FUERZA CENTRÍFGA ------------
        btVector3 rightVector = vc.m_carChassis->getWorldTransform().getBasis() * btVector3(-vc.initDirectionDrift/0.3f, 0, 0);
        vc.m_carChassis->applyCentralImpulse(rightVector * (vc.storedVelocity.length() * FACTOR_CETRIFUGAL_FORCE));
        // -------------------------------------------------

        
    } else {
        // --------- Aplicar boost al finalizar el derrape --------
        if (vc.stateVehicle == StateVehicle::PENDINGBOOSTDRIFT)
        {
            applyImpulseBoost(vc, vc.storedBoostLastDrift);
            EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 2));
          

            // Reiniciar boost
            //vc.storedBoostLastDrift = 0;
            vc.stateVehicle = StateVehicle::BOOSTDRIFT;
        }

        if(groundT == GroundType::BOOST)
        {
            if(vc.m_carChassis->getLinearVelocity().length() < MAX_SPEED_BOOST)
            {
                applyImpulseBoost(vc, 1.5f);
                vc.stateVehicle = StateVehicle::BOOSTDRIFT;
            }
        }
        // -------------------------------------------------------

        // ------------- Control normal del vehículo ---------------
        StateVehicle stateVehicle = vc.stateVehicle;
        btVector3 velocity = vc.m_carChassis->getLinearVelocity();
        float speed = velocity.length();
        
        float factorIncreaseSpeed = 0.35f;
        float maxSpeedMultiplier = 1.0f + (((vc.posicion - 1)/6.0f) * factorIncreaseSpeed);
        float maxSpeedPosition = maxSpeedMultiplier * vc.maxSpeed;

        if(vc.maxSpeedInGame < maxSpeedPosition){
            vc.maxSpeedInGame *= (2 - 0.9995f);
        }else{
            vc.maxSpeedInGame *= 0.9995f;
        }
        //if(e.id == 147)
        // if(vc.posicion != 6)
        float speedKPH = vc.m_vehicle->getCurrentSpeedKmHour();


        if(speed > vc.maxSpeedInGame)
        {
            if(stateVehicle != StateVehicle::BOOSTDRIFT)    // Limitar la velocidad
            {
                btVector3 limitedVelocity = velocity.normalized() * vc.maxSpeedInGame;
                vc.m_carChassis->setLinearVelocity(limitedVelocity);                                
            }
            else
            {
                if(brake == 0)
                {
                    vc.storedVelocityAfterImpulse *= powf(FACTOR_REDUCTION_SPEED_MAX_BOOST, deltaTime * 60.0f);
                    vc.m_carChassis->setLinearVelocity(velocity.normalized() * vc.storedVelocityAfterImpulse.length());
                }                
            }

            vc.m_vehicle->applyEngineForce(0 * vc.engineForce, 2); 
            vc.m_vehicle->applyEngineForce(0 * vc.engineForce, 3);

        }
        else {
            if(vc.stateVehicle == StateVehicle::BOOSTDRIFT)
            {
                vc.stateVehicle = StateVehicle::NORMAL;
            }
        }
        
        if(brake == 0)
        {
            for (int i = 0; i < 4; i++)
                vc.m_vehicle->setBrake(0, i);
            
            vc.m_vehicle->applyEngineForce(throttle * vc.engineForce, 2); 
            vc.m_vehicle->applyEngineForce(throttle * vc.engineForce, 3);
        }
        else 
        {

            if(speedKPH > 0.1f)
            {
                for (int i = 0; i < 4; i++) 
                    vc.m_vehicle->setBrake(brake * vc.brakeForce, i);
                    vc.m_vehicle->applyEngineForce(0 * vc.engineForce, 2); 
                    vc.m_vehicle->applyEngineForce(0 * vc.engineForce, 3);
            }
            else 
            {
                
                for (int i = 0; i < 4; i++)
                    vc.m_vehicle->setBrake(0, i);

                vc.m_vehicle->applyEngineForce(-brake * vc.engineForce, 2);  
                vc.m_vehicle->applyEngineForce(-brake * vc.engineForce, 3);  

                vc.stateVehicle = StateVehicle::REVERSE;
            }
        }
        
        vc.m_vehicle->setSteeringValue(vc.steering, 0); // Dirección en rueda delantera derecha
        vc.m_vehicle->setSteeringValue(vc.steering, 1); // Dirección en rueda delantera izquierda
        // -------------------------------------------------------

        if(vc.stateVehicle == StateVehicle::REVERSE && speedKPH >= 0)
        {
            vc.stateVehicle = StateVehicle::NORMAL;
        }
    }
    
    updatePowerUpVehicle(e, EVM, deltaTime);
};

void VehicleSystem::applyImpulseBoost(VehicleComponent& vc, float boost)
{
    btVector3 forward = vc.m_carChassis->getWorldTransform().getBasis() * btVector3(0, 0, 1);
    btVector3 currentVelocity = vc.m_carChassis->getLinearVelocity();
    float currentSpeed = currentVelocity.length();

    // Calcular impulso deseado
    btVector3 desiredImpulse = forward * ((boost * boost) * vc.engineForce);
    vc.m_carChassis->applyCentralImpulse(desiredImpulse);

    // Consultar la velocidad real después del impulso
    btVector3 afterVelocity = vc.m_carChassis->getLinearVelocity();
    float realNewSpeed = afterVelocity.length();

    // Si la nueva velocidad supera el máximo permitido, se limita
    float maxAllowedSpeed = MAX_SPEED_BOOST;
    if (realNewSpeed > maxAllowedSpeed) 
    {
        btVector3 limitedVelocity = afterVelocity.normalized() * maxAllowedSpeed;
        vc.m_carChassis->setLinearVelocity(limitedVelocity);
    }

    vc.storedVelocityAfterImpulse = vc.m_carChassis->getLinearVelocity();
}

void VehicleSystem::syncOnePlayer(E& e)
{
    auto& m = e.getParent().getComponent<MultiplayerComponent>(e.getComponentKey<MultiplayerComponent>().value());
    auto& v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value()); 
    auto& ownV = e.getParent().getComponent<MultiplayerComponent>(e.getParent().getEntityByType(EntityType::PLAYER)->getComponentKey<MultiplayerComponent>().value());

    // COMPORBAR SI AÑADIR EL TAM TAMBIEN

    if(m.checkSync && (ownV.idServer != m.idServer))
    {
        v.m_carChassis->getWorldTransform().setOrigin(btVector3(m.dataSaves.position.x, m.dataSaves.position.y, m.dataSaves.position.z));
        v.m_carChassis->getWorldTransform().setRotation(btQuaternion(m.dataSaves.rotation.axis.x, m.dataSaves.rotation.axis.y, m.dataSaves.rotation.axis.z, m.dataSaves.rotation.angle));
        v.m_carChassis->setLinearVelocity(btVector3(m.dataSaves.linearVelocity.x, m.dataSaves.linearVelocity.y, m.dataSaves.linearVelocity.z));
        v.m_carChassis->setAngularVelocity(btVector3(m.dataSaves.angularVelocity.x, m.dataSaves.angularVelocity.y, m.dataSaves.angularVelocity.z));

        v.powerUp    = m.poweUpServer;
        v.tamPowerUp = m.tamPowerUp;
        
        m.checkSync = false;
    }
}

void VehicleSystem::delete_one_entity(E& e, btDynamicsWorld* dynamicsWorld){
    dynamicsWorld->removeVehicle(e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value()).m_vehicle.get());
};


void VehicleSystem::updatePowerUpVehicle(E& e, EventManager& EVM, float deltaTime){
    auto& v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());

    switch (v.powerUpUsed)
    {
        case PowerUps::BOOST:
        {
            if(v.drifting){
                btVector3 currentDirection = v.m_carChassis->getLinearVelocity().normalized();
                v.storedVelocity = currentDirection * MAX_SPEED_BOOST;
                v.storedVelocityAfterImpulse = v.storedVelocity;
                v.boostWithDrift = true;
                v.stateVehicle = StateVehicle::BOOSTDRIFT;

                v.powerUpUsed = PowerUps::ANYONE;

                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 2));  

                break;
            }

            btVector3 forward = v.m_carChassis->getWorldTransform().getBasis() * btVector3(0, 0, 1);
            
            float boostPowerUp = 2.5f;
            btVector3 desiredImpulse = forward * ((boostPowerUp * boostPowerUp) * v.engineForce);
            v.m_carChassis->applyCentralImpulse(desiredImpulse);

            btVector3 afterVelocity = v.m_carChassis->getLinearVelocity();
            float realNewSpeed = afterVelocity.length();
            float maxAllowedSpeed = MAX_SPEED_BOOST;
            if (realNewSpeed > maxAllowedSpeed)              // Si la nueva velocidad supera el máximo permitido, se limita
            {
                btVector3 limitedVelocity = afterVelocity.normalized() * maxAllowedSpeed;
                v.m_carChassis->setLinearVelocity(limitedVelocity);
            }
            v.storedVelocityAfterImpulse = v.m_carChassis->getLinearVelocity();

            v.stateVehicle = StateVehicle::BOOSTDRIFT;

            v.driftBoost = 0.0f;
            v.powerUpUsed = PowerUps::ANYONE;

            EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 2));

            break;
        }
        case PowerUps::COHETE:
        {
            std::chrono::_V2::system_clock::time_point ahora = std::chrono::high_resolution_clock::now();
            float elapsedTime = std::chrono::duration<float>(ahora - v.timerPowerUp).count();

                if(elapsedTime > 7.0f){
                    v.powerUpUsed = PowerUps::ANYONE;
                    v.maxSpeed = MAXSPEEDPLAYER;

                    if (e.getParent().hasComponent<AIComponent>(e)) {
                        e.getParent().delete_one_component<AIComponent>(e);
                    }

                    e.getParent().addComponent<InputComponent>(e);
                }

                break;
        }
        case PowerUps::BOOSTX3:
        case PowerUps::BANANA:
        case PowerUps::BANANAX3:
        case PowerUps::SHELL:
        case PowerUps::SHELLX3:
        case PowerUps::ANYONE:
            break;
    
    }

}
void VehicleSystem::convertInputs(VehicleComponent& vc, myInput actionInput, float& throttle, float& brake, float deltaTime){

    // ····················· GESTIÓN DE LOS INPUTS ·····························
    if (actionInput.inputMask & GameActions::ACTION_ACCELERATE){
        if(actionInput.R2 != 0){
            //Mando o IA
            throttle = (actionInput.R2 + 1.0f)/2.0f;
        }else{
            //Teclado
            throttle = 1.0f;
        }
    } 
    if (actionInput.inputMask & GameActions::ACTION_BRAKE) {
        if(actionInput.L2 != 0){
            //Mando o IA
            brake = (actionInput.L2 + 1.0f)/2.0f;
        }else{
            //Teclado
            brake = 1.0f;
        }
    }
    if (actionInput.inputMask & GameActions::ACTION_LEFT){
        if(actionInput.LJ != 0){
            //Mando o IA
            vc.steering = -actionInput.LJ*MAX_STEERING_NORMAL;
        }else{
            //Teclado
            if(vc.steering < 0){
                vc.steering = 0;
            }
            //if(vc.steering < MAX_STEERING_NORMAL){
            if(vc.steering < vc.max_steering_normal){
                vc.steering += (FACTOR_INCREASE_STEERING_NORMAL * 60.0f) * deltaTime;
            } 
            //if (vc.steering > MAX_STEERING_NORMAL) vc.steering = MAX_STEERING_NORMAL;
            if (vc.steering > vc.max_steering_normal) vc.steering = vc.max_steering_normal;
        }
    }
    if (actionInput.inputMask & GameActions::ACTION_RIGHT){
        if(actionInput.LJ != 0){
            //Mando o IA
            vc.steering = -actionInput.LJ*MAX_STEERING_NORMAL;
        }else{
            //Teclado
            if(vc.steering > 0){
                vc.steering = 0;
            }
            //if(vc.steering > -MAX_STEERING_NORMAL){
            if(vc.steering > -vc.max_steering_normal){
                vc.steering -= (FACTOR_INCREASE_STEERING_NORMAL * 60.0f) * deltaTime;
            } 
            //if (vc.steering < -MAX_STEERING_NORMAL) vc.steering = -MAX_STEERING_NORMAL;
            if (vc.steering < -vc.max_steering_normal) vc.steering = -vc.max_steering_normal;
        }
    }
    if(!(actionInput.inputMask & GameActions::ACTION_LEFT) && !(actionInput.inputMask & GameActions::ACTION_RIGHT)){
        vc.steering = 0;
    }
// ·····················································································
}
void VehicleSystem::testRayGround(VehicleComponent& vc, GroundType& groundT, int& idEntity){
    auto carTransform = vc.m_carChassis->getWorldTransform();

    btVector3 centerPosition = vc.m_carChassis->getWorldTransform().getOrigin();
    btVector3 centerRayEnd = centerPosition - btVector3(0, 2.0f, 0);  // Hacia abajo 7 unidades

    btVector3 wheelOffsets[] = {
        {1.0f, -0.5f, 1.5f},   // Delantera derecha
        {-1.0f, -0.5f, 1.5f},  // Delantera izquierda
        {1.0f, -0.5f, -1.5f},  // Trasera derecha
        {-1.0f, -0.5f, -1.5f}  // Trasera izquierda
    };

    vc.m_vehicle->getWheelInfo(0).m_worldTransform.getOrigin();

    btVector3 rayDirection {0, -1, 0};
    float rayLenght {2.0f};

    GroundType resultGround = GroundType::NORMAL;
    int foundBoostId = -1;
    bool foundGround = false;


    // Lambda para realizar pruebas de rayos
    auto performRayTest = [&](const btVector3 &start, const btVector3 &end, GroundType& resultGround)
    {
        
        btCollisionWorld::ClosestRayResultCallback callback(start, end);
        vc.dynamic_world_parent->rayTest(start, end, callback);

        if (callback.hasHit())
        {
            btCollisionObject *hitObject = const_cast<btCollisionObject *>(callback.m_collisionObject);
            ObjectRigidType type = static_cast<ObjectRigidType>(hitObject->getUserIndex());
            int idEntityHit = hitObject->getUserIndex3();


            const char* objectName = "Desconocido";  // Valor por defecto

            
            switch (type)
            {
                case ObjectRigidType::TYPE_GROUND_INFINITY:
                    objectName = "Suelo infinito";
                    //groundT = GroundType::SLOWDOWN;
                    resultGround = GroundType::SLOWDOWN;
                    break;
                case ObjectRigidType::TYPE_GROUND:
                    objectName = "Carretera";
                    //groundT = GroundType::NORMAL;
                    resultGround = GroundType::NORMAL;
                    break;
                case ObjectRigidType::TYPE_BOOST_GROUND:
                    objectName = "BOOSTGROUND";
                    //groundT = GroundType::BOOST;
                    resultGround = GroundType::BOOST;
                    idEntity = idEntityHit;
                    break;
                default:
                    objectName = "Objeto desconocido";
                    break;
            }
            
        }
    };

   
    GroundType resultGroundCenter = GroundType::NORMAL;

    GroundType resultGroundWheels[4] = {
        GroundType::NORMAL,
        GroundType::NORMAL,
        GroundType::NORMAL,
        GroundType::NORMAL
    };

    btVector3 crayCenter = carTransform.getOrigin();
    performRayTest(centerPosition, centerRayEnd, resultGroundCenter);
    if(resultGroundCenter == GroundType::SLOWDOWN)
    {
        groundT = GroundType::SLOWDOWN;
        return;
    }
    else if(resultGroundCenter == GroundType::BOOST)
    {
        groundT = GroundType::BOOST;
        return;
    }

    int id {0};
    //for(const btVector3& offset : wheelOffsets)
    //{
    //    btVector3 wheelPosition = carTransform.getOrigin() + offset;
    //    performRayTest(wheelPosition, wheelPosition + (rayDirection * rayLenght), resultGroundWheels[id]);
    //    id++;
    //}


    for(int i=0; i<4; i++)
    {
        btVector3 wheelPosition = vc.m_vehicle->getWheelInfo(i).m_worldTransform.getOrigin();
        btVector3 endPos = wheelPosition - btVector3(0, 2.0f, 0);
        performRayTest(wheelPosition, endPos, resultGroundWheels[id]);
        id++;
    }

   
    for(const GroundType ground : resultGroundWheels)
    {
        if(ground == GroundType::BOOST)
        {
            groundT = GroundType::BOOST;
            break;
        }
    }
     
}