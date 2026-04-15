#include "VehicleInputSystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../man/EventManager.hpp"

#include "../util/EnumsActions.hpp"


void VehicleInputSystem::update(EManager& EM, EventManager& EVM, float deltaTime){
    EM.forAllAux1<void(*)(E&, EventManager&, float)>(update_one_entity, EVM, deltaTime);
};

void VehicleInputSystem::update_one_entity(E& e, EventManager& EVM, float deltaTime)
{
    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());

    // -------------- GESTIÓN DE LOS INPUTS --------------
    myInput inputMask {};
    
    if(e.getParent().hasComponent<InputComponent>(e))
    {
        inputMask = e.getParent().getComponent<InputComponent>(e.getComponentKey<InputComponent>().value()).actionInput;    
    }
    else if(e.getParent().hasComponent<AIComponent>(e))
    {
        inputMask = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value()).actionInput;
    }
    // ------------------------------------------

    update_vehicle_with_inputs(vc, inputMask, deltaTime);
    //if(vc.m_carChassis->getUserIndex2() == 1){
    //    vc.m_carChassis->setUserIndex2(0);
    //    vc.drifting = false;
    //    vc.stateVehicle = StateVehicle::NORMAL;
    //}
    process_events(e);


    if(vc.gozBoost)
    {
        vc.stateVehicle = StateVehicle::BOOSTDRIFT;
        vc.gozBoost = false;
    }

    
    // -------------- GESTIÓN DEL DRIFT --------------
    if (!vc.drifting) 
    {
        vc.drifting = (inputMask.inputMask & GameActions::ACTION_DRIFT) && vc.steering != 0 && (vc.m_carChassis->getLinearVelocity().length() > (FACTOR_MINIMUM_SPEED_DRIFT * vc.maxSpeed));
        vc.initDirectionDrift = 0.0f;
        if (vc.drifting) {
            vc.initDirectionDrift = vc.steering;     // -0.3 o 0.3
            vc.storedVelocity = vc.m_carChassis->getLinearVelocity(); // Almacenar velocidad inicial
            vc.steeringDrift = 0.0f;
            vc.previousStateDrift = vc.stateVehicle;
            //vc.stateVehicle = StateVehicle::DRIFTING;

            e.getParent().addParticleRequest(e.id, ParticleTypes::CLOUD);
            e.getParent().addParticleRequest(e.id, ParticleTypes::SPARK);
            
            EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 1));

        }
    }

    if (vc.drifting)  // Control de final de drift
    {
        vc.drifting = ((inputMask.inputMask & GameActions::ACTION_DRIFT) && (inputMask.inputMask & GameActions::ACTION_ACCELERATE) && (!(inputMask.inputMask & GameActions::ACTION_BRAKE)));
        if(!vc.drifting){
            vc.steeringDrift = 0.0f;
            vc.storedBoostLastDrift = vc.driftBoost;
            vc.driftBoost = 0.0f;
            vc.storedVelocity = { 0.0f, 0.0f, 0.0f };

            if(vc.storedBoostLastDrift > 1.0f)
            {
                vc.stateVehicle = StateVehicle::PENDINGBOOSTDRIFT;
            }
            else if (vc.stateVehicle == StateVehicle::BOOSTDRIFT)
            {
                vc.stateVehicle = StateVehicle::BOOSTDRIFT;
            }
            else
            {
                vc.stateVehicle = StateVehicle::NORMAL;
            } 
            EVM.addEvent(EventTypes::STOPSOUND, std::make_unique<StopSoundData>(e.id, 1));
        }
        if(vc.drifting && (vc.storedVelocity.length() < MIN_SPEED_TO_END_DRIFT))
        {
            vc.drifting = false;
            vc.driftBoost = 0;
            vc.stateVehicle = StateVehicle::NORMAL;
            EVM.addEvent(EventTypes::STOPSOUND, std::make_unique<StopSoundData>(e.id, 1));
        }
    }

    
    // ------------------------------------------
    
    // -------------- GESTIÓN DEl MOVIMIENTO --------------
    if (vc.drifting) 
    {
        // ----------- AJUSTAR ANGULO DE GIRO ------------
        if(vc.steering != 0 && (copysign(1.0, vc.initDirectionDrift) == copysign(1.0, vc.steering)))
        {
            float steeringTarget = (vc.initDirectionDrift > 0) ? vc.max_steering_drift : -vc.max_steering_drift;

            vc.steeringDrift = Lerp(vc.steeringDrift, steeringTarget, VELOCITY_LERP * deltaTime);
        }

        else if(fabs(vc.steering) < STEERING_WITHOUT)
        {
            if(vc.initDirectionDrift > 0)
            {
                if(vc.steeringDrift > (vc.max_steering_normal + 0.01)) vc.steeringDrift -= (FACTOR_RETURN_STEERING_ANYKEY * 60.0f) * deltaTime; 
                else if(vc.steeringDrift < (vc.max_steering_normal - 0.01)) vc.steeringDrift += (FACTOR_RETURN_STEERING_ANYKEY * 60.0f) * deltaTime;
                else vc.steeringDrift = vc.max_steering_normal;
            }
            else if(vc.initDirectionDrift < 0)
            {
                if(vc.steeringDrift < -(vc.max_steering_normal + 0.01)) vc.steeringDrift += (FACTOR_RETURN_STEERING_ANYKEY * 60.0f) * deltaTime;
                else if(vc.steeringDrift >-(vc.max_steering_normal - 0.01)) vc.steeringDrift -= (FACTOR_RETURN_STEERING_ANYKEY * 60.0f) * deltaTime;
                else vc.steeringDrift = -vc.max_steering_normal;
            }
        }
        else
        {
            if(vc.initDirectionDrift > 0 && vc.steering < 0)    // Steering no es 0 ni la misma dirección inicial
            {
                if(vc.steeringDrift > vc.min_steering_drift) vc.steeringDrift -= (FACTOR_INCREASE_STEERING_DRIFT * 60.0f) * deltaTime;
                else vc.steeringDrift = vc.min_steering_drift;
            }
            else if(vc.steeringDrift < 0 && vc.steering > 0)
            {
                if(vc.steeringDrift < -vc.min_steering_drift) vc.steeringDrift += (FACTOR_INCREASE_STEERING_DRIFT * 60.0f) * deltaTime;
                else vc.steeringDrift = -vc.min_steering_drift;
            }
        }
        // -----------------------------------------------
        // ----------- ACUMULAR BOOST DE DERRAPE ------------
        vc.driftBoost += (vc.factor_increase_boost * 60.0f) * deltaTime;
        vc.driftBoost = fmin(vc.driftBoost, MAX_BOOST_DRIFT);
        // --------------------------------------------------
    }
    else 
    {

        //if ( vc.m_carChassis->getLinearVelocity().length() > (vc.maxSpeed + 1.0f) && vc.stateVehicle == StateVehicle::PENDINGBOOSTDRIFT)
        //{
        //    vc.stateVehicle = StateVehicle::BOOSTDRIFT;
        //}

        //if ( vc.m_carChassis->getLinearVelocity().length() < vc.maxSpeed && vc.stateVehicle == StateVehicle::BOOSTDRIFT)
        //{
        //    vc.stateVehicle = StateVehicle::NORMAL;
        //}

        //float speedKPH = vc.m_vehicle->getCurrentSpeedKmHour();
        //if(speedKPH < 0 )
        //    vc.stateVehicle = StateVehicle::REVERSE;
        //else if ( vc.stateVehicle == StateVehicle::REVERSE)
        //    vc.stateVehicle = StateVehicle::NORMAL;
        
    }

    if (inputMask.inputMask & GameActions::ACTION_POWERUP)
    {
        if (vc.powerUp != PowerUps::ANYONE && vc.tamPowerUp > 0)
        {
            e.getParent().addEntityRequest(e);
        }
    }


}

void VehicleInputSystem::update_vehicle_with_inputs(VehicleComponent& vc, myInput actionInput, float deltaTime){

    // ····················· GESTIÓN DE LOS INPUTS ·····························
    vc.throttle = 0.0f;
    vc.brake = 0.0f;


    if (actionInput.inputMask & GameActions::ACTION_ACCELERATE){
        if(actionInput.R2 != 0){
            //Mando o IA
            vc.throttle = (actionInput.R2 + 1.0f)/2.0f;
        }else{
            //Teclado
            vc.throttle = 1.0f;
        }
    } 
    if (actionInput.inputMask & GameActions::ACTION_BRAKE) {
        if(actionInput.L2 != 0){
            //Mando o IA
            vc.brake = (actionInput.L2 + 1.0f)/2.0f;
        }else{
            //Teclado
            vc.brake = 1.0f;
        }
    }
    if (actionInput.inputMask & GameActions::ACTION_LEFT){
        if(actionInput.LJ != 0){
            //Mando o IA
            vc.steering = -actionInput.LJ * MAX_STEERING_NORMAL;
        }else{
            //Teclado
            if(vc.steering < 0){
                vc.steering = 0;
            }
            
            if(vc.steering < vc.max_steering_normal){
                vc.steering += (FACTOR_INCREASE_STEERING_NORMAL * 60.0f) * deltaTime;
            } 
            
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
            
            if(vc.steering > -vc.max_steering_normal){
                vc.steering -= (FACTOR_INCREASE_STEERING_NORMAL * 60.0f) * deltaTime;
            } 
            
            if (vc.steering < -vc.max_steering_normal) vc.steering = -vc.max_steering_normal;
        }
    }
    if(!(actionInput.inputMask & GameActions::ACTION_LEFT) && !(actionInput.inputMask & GameActions::ACTION_RIGHT)){
        vc.steering = 0;
    }
// ·····················································································
}

float VehicleInputSystem::Lerp(float a, float b, float t) 
{
    return a + (b - a) * t;
}

void VehicleInputSystem::process_events(E& e)
{
    auto& ec = e.getParent().getComponent<EventComponent>(e.getComponentKey<EventComponent>().value());
    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto const vectorEventCollisionWall = ec.getEventDataType<CollisionWithWallData>(); // Todos se refieren a la propia entidad

    for (const auto* event : vectorEventCollisionWall)
    {
        vc.drifting = false;
        vc.stateVehicle = StateVehicle::NORMAL;
    }

    ec.deleteEventsType<CollisionWithWallData>();
}
