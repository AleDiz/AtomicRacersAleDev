#include "VehicleComponent.hpp"
#include "../util/ObjectRigidType.hpp"

void VehicleComponent::setVehicleConfigurationValues(std::unordered_map<std::string, float> configuration){
    maxSpeed = configuration["max_speed"];
    maxSpeedInGame = maxSpeed;
    engineForce = configuration["engine_force"];
    damping = configuration["damping"];
    max_steering_normal = configuration["max_steering_normal"];
    max_steering_drift = configuration["max_steering_drift"];
    min_steering_drift = configuration["min_steering_drift"];
    brakeForce = configuration["brake_force"];
    mass = configuration["mass"];
    factor_increase_boost = configuration["factor_increase_boost"];
    wheel_radious = configuration["wheel_radious"];
    suspension_rest_length = configuration["suspension_rest_length"];
    
}

void VehicleComponent::initPhysics(btDynamicsWorld* dynamicsWorld, btVector3 positionInitial){


    dynamic_world_parent = dynamicsWorld;

    // Crear el chasis del coche
    chassisShape = std::make_unique<btBoxShape>(btVector3(1.0f, 0.5f, 2.0f));
    vehicleMotionState = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion(0, 0, 0, 1), positionInitial)); // Elevado del suelo

    //btScalar mass = 800.0f;
    btVector3 inertia(0, 0, 0);
    chassisShape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo chassisInfo(mass, vehicleMotionState.get(), chassisShape.get(), inertia);
    m_carChassis = std::make_unique<btRigidBody>(chassisInfo);
    m_carChassis->setAngularFactor(btVector3(0, 1, 0));
    m_carChassis->forceActivationState(DISABLE_DEACTIVATION);
    dynamicsWorld->addRigidBody(m_carChassis.get());

    // Crear raycaster y vehículo
    m_vehicleRaycaster = std::make_unique<btDefaultVehicleRaycaster>(dynamicsWorld);
    btRaycastVehicle::btVehicleTuning tuning;
    tuning.m_suspensionStiffness = 100.0f;   // Más rigidez para evitar inclinación excesiva
    tuning.m_suspensionDamping = 4.0f;      // Reduce rebotes
    tuning.m_suspensionCompression = 5.0f;  // Mayor resistencia a la compresión
    tuning.m_maxSuspensionTravelCm = 5.0f; // Limitar el recorrido de la suspensión
    m_vehicle = std::make_unique<btRaycastVehicle>(tuning, m_carChassis.get(), m_vehicleRaycaster.get());
    m_vehicle->setCoordinateSystem(0, 1, 2);
    dynamicsWorld->addVehicle(m_vehicle.get());

    m_carChassis->setUserIndex(ObjectRigidType::TYPE_CHASIS_CAR);
    m_carChassis->setCollisionFlags(m_carChassis->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    interpolatedTransform.setOrigin(m_carChassis->getWorldTransform().getOrigin());
    interpolatedTransform.setRotation(m_carChassis->getWorldTransform().getRotation());

    // Configurar ruedas
    setupWheels();
}

void VehicleComponent::clearComponent(){
    dynamic_world_parent->removeVehicle(m_vehicle.get());
    dynamic_world_parent->removeCollisionObject(m_carChassis.get());
}

btDynamicsWorld* VehicleComponent::getParent() {
    return dynamic_world_parent;
}

btDynamicsWorld* VehicleComponent::getParent() const {
    return dynamic_world_parent;
}

void VehicleComponent::setupWheels() {
    btVector3 wheelDirectionCS0(0, -1, 0); // Hacia abajo
    btVector3 wheelAxleCS(-1, 0, 0);       // Eje de rotación
    //float suspensionRestLength = 0.6f;
    //float wheelRadius = 0.5f;

    // Posiciones de las ruedas
    btVector3 wheelOffsets[] = {
        {1.0f, -0.5f, 1.5f},   // Delantera derecha
        {-1.0f, -0.5f, 1.5f},  // Delantera izquierda
        {1.0f, -0.5f, -1.5f},  // Trasera derecha
        {-1.0f, -0.5f, -1.5f}  // Trasera izquierda
    };

    for (int i = 0; i < 4; i++) {
        bool isFrontWheel = (i < 2);
        m_vehicle->addWheel(wheelOffsets[i], wheelDirectionCS0, wheelAxleCS, suspension_rest_length, wheel_radious, btRaycastVehicle::btVehicleTuning(), isFrontWheel);
        btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
        wheel.m_rollInfluence = 0.03;
        wheel.m_frictionSlip = 10.0f; // Fricción de la rueda
    }
}

myVector3 VehicleComponent::getWheelRightBackPos(){
    btVector3 wheelOffsets = m_vehicle->getWheelInfo(2).m_worldTransform.getOrigin();

    myVector3 wheel = myVector3{
        wheelOffsets.getX(), 
        wheelOffsets.getY(), 
        wheelOffsets.getZ()
    };

    return wheel;
}

myVector3 VehicleComponent::getWheelLeftBackPos(){
    btVector3 wheelOffsets = m_vehicle->getWheelInfo(3).m_worldTransform.getOrigin();

    myVector3 wheel = myVector3{
        wheelOffsets.getX(), 
        wheelOffsets.getY(), 
        wheelOffsets.getZ()
    };

    return wheel;
}
