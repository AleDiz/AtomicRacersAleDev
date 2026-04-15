#include "AISystem.hpp"

#include <numbers>
#include <cmath>
#include <random>

#include "../util/EnumsActions.hpp"

#include "../Entity/Entity.hpp"                // Entidades
#include "../man/EntityManager.hpp"
#include "../components/VehicleComponent.hpp"
#include "../components/WaypointComponent.hpp"

const float TURN_THRESHOLD = 0.08f; // Umbral en radianes (ajustable)

void AISystem::arrive(E &e, float tx, float tz)
{
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aic = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    btTransform transform = v.m_carChassis->getWorldTransform();
    btVector3 position = transform.getOrigin();

    // Diferencias en posición
    float dx = tx - position.getX();
    float dz = tz - position.getZ();
    float distanceSq = dx * dx + dz * dz;

    // Verificar si estamos dentro del radio de llegada
    
    float distance = std::sqrt(distanceSq);

    if(distance > aic.arrivalRadius){
        float linearSpeed = std::clamp(distance / aic.time2arrive, 0.0f, v.maxSpeed);
        
        if (linearSpeed == v.maxSpeed)
        {
            aic.actionInput.inputMask |= GameActions::ACTION_ACCELERATE;
            aic.actionInput.R2 = 0.0f;
        }else{
            aic.actionInput.inputMask |= GameActions::ACTION_ACCELERATE;
            aic.actionInput.R2 = std::clamp(linearSpeed/v.maxSpeed - 1.0f, -0.8f, 0.0f);
        }

        float desiredAngle = std::atan2(dz, dx);
        auto forward = v.m_vehicle->getForwardVector();
        btVector3 forwardXZ = forward;
        forwardXZ.setY(0); // Asegurarse de que el vector esté en el plano XZ
        forwardXZ.normalize();

        if (desiredAngle < 0)
            desiredAngle += static_cast<float>(2.0f * M_PI);

        float currentAngle = std::atan2(forwardXZ.getZ(), forwardXZ.getX());

        // Diferencia angular (ajustar al rango [-PI, PI])
        float angleDiff = desiredAngle - currentAngle;
        if (angleDiff > M_PI)
            angleDiff -= static_cast<float>(2.0f * M_PI);
        if (angleDiff < -M_PI)
            angleDiff += static_cast<float>(2.0f * M_PI);

        // Aplicar la lógica de giro solo si la diferencia angular supera el umbral
        if (angleDiff < 0 && distance > 1.5f) {
            aic.actionInput.inputMask |= GameActions::ACTION_LEFT;  
            aic.actionInput.LJ = -std::clamp(std::abs(angleDiff)/v.max_steering_normal, 0.0f, 1.0f);
        } else if (angleDiff > 0 && distance > 1.5f){
            aic.actionInput.inputMask |= GameActions::ACTION_RIGHT;
            aic.actionInput.LJ = std::clamp(angleDiff/v.max_steering_normal, 0.0f, 1.0f);
        }

        if(aic.initDriftDirection != 0 && (std::signbit(aic.initDriftDirection) != std::signbit(angleDiff))){
            aic.actionInput.inputMask &= ~GameActions::ACTION_DRIFT;
            aic.initDriftDirection = 0;
        }else if (std::abs(angleDiff) > v.max_steering_drift)
        {
            
            aic.actionInput.inputMask |= GameActions::ACTION_DRIFT;
            aic.initDriftDirection = angleDiff;
        }else{
            aic.initDriftDirection = 0;
        }
    }else{
        aic.waypoint2Go++;
        if(aic.waypoint2Go >= e.getParent().getEntitiesAux2().size()){
            aic.waypoint2Go = 0;
        }
        calculateClosestCoord(e);
    }    
}

void AISystem::seek(E &e, float tx, float tz, int target)
{
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aic = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    btTransform transform = v.m_carChassis->getWorldTransform();
    btVector3 position = transform.getOrigin();

    // Dirección hacia el objetivo (seek) o en dirección contraria (flee)
    float dx = tx - position.getX();
    float dz = tz - position.getZ();

    if (target <= 0) {
        dx = -dx;
        dz = -dz;
    }

    float desiredAngle = std::atan2(dz, dx);
    auto forward = v.m_vehicle->getForwardVector();
    btVector3 forwardXZ = forward;
    forwardXZ.setY(0);
    forwardXZ.normalize();

    if (desiredAngle < 0)
        desiredAngle += static_cast<float>(2.0f * M_PI);

    float currentAngle = std::atan2(forwardXZ.getZ(), forwardXZ.getX());

    // Diferencia angular (ajustar al rango [-PI, PI])
    float angleDiff = desiredAngle - currentAngle;
    if (angleDiff > M_PI)
        angleDiff -= static_cast<float>(2.0f * M_PI);
    if (angleDiff < -M_PI)
        angleDiff += static_cast<float>(2.0f * M_PI);

    // Movimiento hacia/desde el objetivo
    aic.actionInput.inputMask |= GameActions::ACTION_ACCELERATE;
    aic.actionInput.R2 = 0.0f;

    // Lógica de giro
    if (angleDiff < 0) {
        aic.actionInput.inputMask |= GameActions::ACTION_LEFT;
        aic.actionInput.LJ = -std::clamp(std::abs(angleDiff) / v.max_steering_normal, 0.0f, 1.0f);
    } else if (angleDiff > 0) {
        aic.actionInput.inputMask |= GameActions::ACTION_RIGHT;
        aic.actionInput.LJ = std::clamp(angleDiff / v.max_steering_normal, 0.0f, 1.0f);
    }

    // Derrape si el ángulo es suficientemente pronunciado
    if (aic.initDriftDirection != 0 && (std::signbit(aic.initDriftDirection) != std::signbit(angleDiff))) {
        aic.initDriftDirection = 0;
        aic.actionInput.inputMask &= ~GameActions::ACTION_DRIFT;
    } else if (std::abs(angleDiff) > v.max_steering_normal) {
        aic.actionInput.inputMask |= GameActions::ACTION_DRIFT;
        aic.initDriftDirection = angleDiff;
    } else {
        aic.initDriftDirection = 0;
    }
}

void AISystem::flee(E &e, float tx, float tz)
{
    seek(e, tx, tz, -1);
}

void AISystem::align(E &e, float tx, float tz, bool backwards)
{
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aic = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    btTransform transform = v.m_carChassis->getWorldTransform();
    btVector3 position = transform.getOrigin();

    float dx = tx - position.getX();
    float dz = tz - position.getZ();

    float desiredAngle = std::atan2(dz, dx);

    btVector3 forward = v.m_vehicle->getForwardVector();
    btVector3 forwardXZ = forward;
    forwardXZ.setY(0);
    forwardXZ.normalize();

    float currentAngle = std::atan2(forwardXZ.getZ(), forwardXZ.getX());

    // Asegurar que ambos ángulos estén en [0, 2π]
    if (desiredAngle < 0)
        desiredAngle += static_cast<float>(2.0f * M_PI);
    if (currentAngle < 0)
        currentAngle += static_cast<float>(2.0f * M_PI);

    // Diferencia angular ajustada a [-PI, PI]
    float angleDiff = desiredAngle - currentAngle;
    if (angleDiff > M_PI)
        angleDiff -= static_cast<float>(2.0f * M_PI);
    if (angleDiff < -M_PI)
        angleDiff += static_cast<float>(2.0f * M_PI);

    // Si el ángulo es pequeño, no hace falta girar
    if (std::abs(angleDiff) < 0.05f) {
        aic.actionInput.LJ = 0.0f;
        return;
    }

    // Aplicar dirección de giro
    if(!backwards){
        if (angleDiff < 0) {
            aic.actionInput.inputMask |= GameActions::ACTION_LEFT;
            aic.actionInput.LJ = -std::clamp(std::abs(angleDiff) / v.max_steering_normal, 0.0f, 1.0f);
        } else {
            aic.actionInput.inputMask |= GameActions::ACTION_RIGHT;
            aic.actionInput.LJ = std::clamp(angleDiff / v.max_steering_normal, 0.0f, 1.0f);
        }
    }else{
        if (angleDiff > 0) {
            aic.actionInput.inputMask |= GameActions::ACTION_LEFT;
            aic.actionInput.LJ = -std::clamp(std::abs(angleDiff) / v.max_steering_normal, 0.0f, 1.0f);
        } else {
            aic.actionInput.inputMask |= GameActions::ACTION_RIGHT;
            aic.actionInput.LJ = std::clamp(angleDiff / v.max_steering_normal, 0.0f, 1.0f);
        }
    }
    
}

void AISystem::update_one_entity(E &e, btDynamicsWorld *dynamicsWorld, bool updateIA, float deltatime)
{
    detectCollisions(dynamicsWorld, e);
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aiComp = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    // auto &renderShape = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
    // renderShape.shape.position = {aiComp.targetCollision.getX(), 10.0f, aiComp.targetCollision.getZ()};
    

    // Refresco del tiempo
    aiComp.accumulatedTime += deltatime;
    if (aiComp.accumulatedTime >= aiComp.time2Update)
    {
        updateScoreActions(e);
        aiComp.accumulatedTime = 0.0f;
    }

    //Recargo cooldown de los powerUp
    aiComp.powerupCooldown -= deltatime;
    if(aiComp.powerupCooldown < 0.0f){
        aiComp.powerupCooldown = 0.0f;
    }

    // Comprobación de los waypoints
    checkWaypoint(e);
    
    
    if (aiComp.tactive && !aiComp.goingBack)
    {
    //LOGICA CON UTILITY (HAY QUE HACER LOS EXECUTE DE CADA ACCION CON LO QUE DEBERIAN HACER)
        aiComp.actionInput = {};
        auto& actionToDo = *aiComp.actions[aiComp.actionIt];
        actionToDo.execute(e);
        
        switch (aiComp.behaviour)
        {
        case SB::PATHFOLLOWING:
        {
             
            if(aiComp.recalculatePoint){
                calculateClosestCoord(e);
                aiComp.recalculatePoint = false;
            }

            //auto &renderShape = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
            //renderShape.shape.position = {aiComp.waypointX, 10.0f, aiComp.waypointZ};
            //renderShape.shape.color = {0, 255, 0, 255};

            arrive(e, aiComp.waypointX, aiComp.waypointZ);
            break;
        }
        case SB::OVERTAKE:
        {
            // lessDistance(e, true);
            aiComp.recalculatePoint = true;
            auto posRival = aiComp.targetCollision;
            auto velRival = v.m_carChassis->getLinearVelocity() * 0.9f;

            float t = aiComp.time2arrive;
            float lateralDistance = aiComp.arrivalRadius;
            auto rayLeft = aiComp.targetTypeTOPLEFT;
            auto rayRight = aiComp.targetTypeTOPRIGHT;

            btVector3 overtakeTarget = PredictOvertakePointFromVel(v.m_carChassis->getWorldTransform().getOrigin(), posRival, velRival, {aiComp.waypointX, 0.0f, aiComp.waypointZ}, t, lateralDistance, rayLeft, rayRight);
            
            //auto &renderShape = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
            //renderShape.shape.position = {overtakeTarget.getX(), 10.0f, overtakeTarget.getZ()};
            //renderShape.shape.color = {0, 0, 255, 255};

            seek(e, overtakeTarget.getX(), overtakeTarget.getZ(), 1);
            break;
        }
        case SB::GROUNDACTION:
        {
            aiComp.recalculatePoint = true;
            if(aiComp.targetTypeGroundL == 3 || aiComp.targetTypeGroundR == 3){
                if(aiComp.targetTypeGroundL == 3 && aiComp.targetTypeGroundR == 3){
                    if(aiComp.targetTypeTOPLEFT == 0 && aiComp.targetTypeTOPRIGHT == 0){
                        aiComp.goingBack = true;
                    }else{
                        arrive(e, aiComp.waypointX, aiComp.waypointZ);
                    }
                }else{
                    flee(e, aiComp.targetCollisionGround.getX(), aiComp.targetCollisionGround.getZ());
                }
            }else if(aiComp.targetTypeGroundL == 4 || aiComp.targetTypeGroundR == 4){
                seek(e, aiComp.targetCollisionGround.getX(), aiComp.targetCollisionGround.getZ(), 1);
            }
            break;
        }
        case SB::AVOID:
        {
            
            aiComp.recalculatePoint = true;

            if(aiComp.targetTypeTOPLEFT == 0 && aiComp.targetTypeTOPRIGHT == 0){
                aiComp.goingBack = true;
            }
            flee(e, aiComp.targetCollision.getX(), aiComp.targetCollision.getZ());
            break;
        }
        case SB::TAKEOBJECT:
        {
            // lessDistance(e, true);
            aiComp.recalculatePoint = true;

            seek(e, aiComp.targetCollision.getX(), aiComp.targetCollision.getZ(), 1);
            break;
        }
        case SB::USEOBJECT:
        {
            if(aiComp.powerupCooldown == 0.0f){
                aiComp.powerupCooldown = 3.0f;
                if(v.powerUp == PowerUps::SHELL){
                    //Align al objetivo
                    aiComp.recalculatePoint = true;
                    align(e, aiComp.targetCollision.getX(), aiComp.targetCollision.getZ(), false);
                    aiComp.actionInput.inputMask |= GameActions::ACTION_POWERUP;
                }else{
                    aiComp.actionInput.inputMask |= GameActions::ACTION_POWERUP;
                }
            }
            break;
        }
        }
    }else if(aiComp.goingBack){
        //Logica para volver al circuito 
        aiComp.timeOut += deltatime;
        aiComp.actionInput.inputMask |= GameActions::ACTION_BRAKE;
        aiComp.actionInput.L2 = 0.0f;
        align(e, aiComp.waypointX, aiComp.waypointZ, true);
        if(aiComp.targetTypeGroundL == 5 && aiComp.targetTypeGroundR == 5 && aiComp.targetTypeTOPLEFT != 0 && aiComp.targetTypeTOPRIGHT != 0){
            aiComp.goingBack = false;
            aiComp.timeOut = 0.0f;
        }else if(aiComp.targetTypeREARLEFT == 0 && aiComp.targetTypeTOPRIGHT == 0){
            aiComp.actionInput.inputMask |= GameActions::ACTION_ACCELERATE;
            aiComp.actionInput.inputMask &= ~GameActions::ACTION_BRAKE;
            aiComp.actionInput.R2 = 0.0f;
        }
        if(aiComp.timeOut > 5.0f){
            aiComp.goingBack = false;
            aiComp.timeOut = 0.0f;

            // // Posición del waypoint actual (donde reaparece)
            // btVector3 currentPos(aiComp.waypointX, v.m_carChassis->getWorldTransform().getOrigin().getY(), aiComp.waypointZ);

            // // Obtener siguiente waypoint
            // auto const& waypoint = e.getParent().getEntityById(e.getParent().getEntitiesAux2()[aiComp.waypoint2Go]);
            // auto const& wComp = waypoint->getParent().getComponent<WaypointComponent>(waypoint->getComponentKey<WaypointComponent>().value());
            // btTransform nextTransform = wComp.rigidBody->getWorldTransform();
            // btVector3 nextPos = nextTransform.getOrigin();

            // // Calcular dirección desde el actual al siguiente waypoint
            // btVector3 direction = nextPos - currentPos;
            // direction.setY(0); // solo en el plano XZ
            // direction.normalize();

            // // Calcular ángulo (yaw) hacia el siguiente waypoint
            // float angle = std::atan2(direction.getZ(), direction.getX());

            // // Crear rotación y nueva transformada
            // btQuaternion rotation(btVector3(0, 1, 0), angle);
            // btTransform newTransform;
            // newTransform.setOrigin(currentPos);
            // newTransform.setRotation(rotation);

            // // Aplicar transformada y resetear físicas
            // v.m_carChassis->setWorldTransform(newTransform);
            // v.m_carChassis->setLinearVelocity(btVector3(0, 0, 0));
            // v.m_carChassis->setAngularVelocity(btVector3(0, 0, 0));
            // v.m_carChassis->clearForces();
        }
    }
}

void AISystem::update(EManager &EM, btDynamicsWorld *dynamicsWorld, bool updateIA, float deltatime)
{

    EM.forAllCondition<void(*)(E&, btDynamicsWorld*, bool, float), AIComponent>(update_one_entity, dynamicsWorld, updateIA, deltatime);

}

btVector3 AISystem::PredictOvertakePointFromVel(
    const btVector3& myPos,
    const btVector3& rivalPos,     // punto detectado por raycast (no el centro del coche rival)
    const btVector3& myReducedVel, // velocidad propia reducida
    const btVector3& targetWaypoint, 
    float t,
    float lateralDistance,
    int rayLeft,
    int rayRight
) {
    // 1. Posición futura desde mi propia velocidad reducida
    btVector3 futurePos = rivalPos + myReducedVel * t;

    // 2. Dirección hacia el waypoint (desde mi posición actual)
    btVector3 dirToWaypoint = targetWaypoint - myPos;
    dirToWaypoint.setY(0);
    if (dirToWaypoint.length2() < 0.001f)
        return futurePos;

    dirToWaypoint.normalize();

    // 3. Vector lateral (izquierda relativa a la dirección)
    btVector3 lateral = dirToWaypoint.cross(btVector3(0, 1, 0)).normalized();

    // 4. Determinar si el waypoint está a la izquierda o derecha del punto de colisión
    btVector3 toWaypointFromRival = targetWaypoint - rivalPos;
    toWaypointFromRival.setY(0);

    float sideSign = lateral.dot(toWaypointFromRival) > 0 ? 1.0f : -1.0f;

    // 5. Crear punto de adelantamiento en el lado del waypoint
    btVector3 overtakePoint = futurePos + lateral * (sideSign * lateralDistance);

    return overtakePoint;
}



void AISystem::detectCollisions(btDynamicsWorld *dynamicsWorld, E &e)
{
    // Obtener la transformacion actual del coche
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &iac = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    auto carTransform = v.m_carChassis->getWorldTransform();
  

    // Longitud de los rayos
    float rayLength = iac.visionDistance;

    // Definir offsets para las esquinas del coche (delanteras y traseras)
    btVector3 frontLeft1Offset(1*0.3, 0, 2*0.6);  
    btVector3 frontLeft2Offset(1*0.6, 0, 2*0.6);  
    btVector3 frontLeft3Offset(1*0.6, 0, 2*0.6-0.3);  

    btVector3 frontRight1Offset(-1*0.3, 0, 2*0.6);
    btVector3 frontRight2Offset(-1*0.6, 0, 2*0.6);
    btVector3 frontRight3Offset(-1*0.6, 0, 2*0.6-0.3);

    btVector3 rearLeftOffset(1*0.8, 0, -2*0.8);
    btVector3 rearRightOffset(-1*0.8, 0, -2*0.8);
    
    btVector3 frontLeftOffsetGL(1*rayLength/2, 0, rayLength);  
    btVector3 frontRightOffsetGR(-1*rayLength/2, 0, rayLength);

    // Calcular posiciones globales para las esquinas
    btVector3 frontLeft1Position = carTransform * frontLeft1Offset;
    btVector3 frontLeft2Position = carTransform * frontLeft2Offset;
    btVector3 frontLeft3Position = carTransform * frontLeft3Offset;

    btVector3 frontRight1Position = carTransform * frontRight1Offset;
    btVector3 frontRight2Position = carTransform * frontRight2Offset;
    btVector3 frontRight3Position = carTransform * frontRight3Offset;

    btVector3 rearLeftPosition = carTransform * rearLeftOffset;
    btVector3 rearRightPosition = carTransform * rearRightOffset;

    btVector3 frontLeftGround = carTransform * frontLeftOffsetGL;
    btVector3 frontRigthGround = carTransform * frontRightOffsetGR;

    // Direcciones diagonales para los rayos
    // Delantera izquierda
    btVector3 diagonalDirectionFL1 = carTransform.getBasis() * btVector3(0,0,1);  
    btVector3 diagonalDirectionFL2 = carTransform.getBasis() * btVector3(0.577f, 0, 1);  // Delantera izquierda
    btVector3 diagonalDirectionFL3 = carTransform.getBasis() * btVector3(0.577f, 0, 1);

    //delantera derecha
    btVector3 diagonalDirectionFR1 = carTransform.getBasis() * btVector3(0,0,1);
    btVector3 diagonalDirectionFR2 = carTransform.getBasis() * btVector3(-0.577f, 0, 1); 
    btVector3 diagonalDirectionFR3 = carTransform.getBasis() * btVector3(-0.577f, 0, 1);


    //atras izquierda
    btVector3 diagonalDirectionRL1 = carTransform.getBasis() * btVector3(0,0,-1);
    btVector3 diagonalDirectionRL2 = carTransform.getBasis() * btVector3(1, 0, -1);
    btVector3 diagonalDirectionRL3 = carTransform.getBasis() * btVector3(1, 0, 0);

    //atras derecha
    btVector3 diagonalDirectionRR1 = carTransform.getBasis() * btVector3(0,0,-1);
    btVector3 diagonalDirectionRR2 = carTransform.getBasis() * btVector3(-1, 0, -1);
    btVector3 diagonalDirectionRR3 = carTransform.getBasis() * btVector3(-1, 0, 0);


    // Rayos al suelo
    btVector3 diagonalDirectionFL_Ground = btVector3(0, -1, 0);
    btVector3 diagonalDirectionFR_Ground = btVector3(0, -1, 0);

    

    // Definir rayos (inicio y fin)
    //izquierda
    btVector3 frontLeftRayEnd = frontLeft1Position + diagonalDirectionFL1.normalized() * rayLength;
    btVector3 frontLeftRayEnd2 = frontLeft2Position + diagonalDirectionFL2.normalized() * rayLength;
    btVector3 frontLeftRayEnd3 = frontLeft3Position + diagonalDirectionFL3.normalized() * rayLength;

    //derecha
    btVector3 frontRightRayEnd = frontRight1Position + diagonalDirectionFR1.normalized() * rayLength;
    btVector3 frontRightRayEnd2 = frontRight2Position + diagonalDirectionFR2.normalized() * rayLength;
    btVector3 frontRightRayEnd3 = frontRight3Position + diagonalDirectionFR3.normalized() * rayLength;

    //atras izquierda
    btVector3 rearLeftRayEnd = rearLeftPosition + diagonalDirectionRL1.normalized() * rayLength;
    btVector3 rearLeftRayEnd2 = rearLeftPosition + diagonalDirectionRL2.normalized() * rayLength;
    btVector3 rearLeftRayEnd3 = rearLeftPosition + diagonalDirectionRL3.normalized() * rayLength;

    //atras derecha
    btVector3 rearRightRayEnd = rearRightPosition + diagonalDirectionRR1.normalized() * rayLength;
    btVector3 rearRightRayEnd2 = rearRightPosition + diagonalDirectionRR2.normalized() * rayLength;
    btVector3 rearRightRayEnd3 = rearRightPosition + diagonalDirectionRR3.normalized() * rayLength;

    //rayos suelo
    btVector3 frontLeftGroundRayEnd = frontLeftGround + diagonalDirectionFL_Ground.normalized() * rayLength;
    btVector3 frontRightGroundRayEnd = frontRigthGround + diagonalDirectionFR_Ground.normalized() * rayLength;
   
    

    // Lambda para realizar pruebas de rayos
    auto performRayTest = [&](const btVector3 &start, const btVector3 &end, float &distance, btVector3 &targetCollisionRay, int &targetType, int num)
    {
        btCollisionWorld::ClosestRayResultCallback callback(start, end);
        dynamicsWorld->rayTest(start, end, callback);

        if (callback.hasHit())
        {
            btCollisionObject *hitObject = const_cast<btCollisionObject *>(callback.m_collisionObject);

            E *collidedEntity = static_cast<E *>(hitObject->getUserPointer());

            distance = callback.m_closestHitFraction * rayLength;
            targetCollisionRay = callback.m_hitPointWorld;


            const char* objectName = "Desconocidoooo";  // Valor por defecto

            if (collidedEntity != nullptr)
            {
                switch (collidedEntity->tipo)
                {
                    case EntityType::PLAYER:
                    case EntityType::IA:
                        objectName = "Coche";
                        targetType = 1;
                        break;
                    case EntityType::OBJETOSMUNDO:
                        objectName = "Muro";
                        targetType = 0;
                        break;
                    case EntityType::POWERUP:
                        objectName = "Power-up";
                        targetType = 2;
                        break;
                    case EntityType::GROUNDINFINITY:
                        objectName = "Suelo slow";
                        targetType = 3;
                        break;
                    case EntityType::BOOSTGROUND:
                        objectName = "Suelo boost";
                        targetType = 4;
                        break;
                    case EntityType::ROAD:
                        objectName = "Carretera";
                        targetType = 5;
                        break;
                    default:
                        objectName = "Objeto desconocido";
                        targetType = -1;
                        break;
                }
                
                
                

            }
            if(targetType==-1){
                distance = 20.0f;
                targetCollisionRay = btVector3(0.0f, 0.0f, 0.0f);
            }
               
        }
        else
        {
            distance = 20.0f;
            targetType = -1;
            targetCollisionRay = btVector3(0.0f, 0.0f, 0.0f);
        }
    };

    // rayos delantera izquierda
    performRayTest(frontLeft1Position, frontLeftRayEnd, iac.rayFrontLeft.distance, iac.rayFrontLeft.rayCollision, iac.rayFrontLeft.type, 1); //recto
    performRayTest(frontLeft2Position, frontLeftRayEnd2, iac.rayDiaLeft.distance, iac.rayDiaLeft.rayCollision, iac.rayDiaLeft.type, 2); //esquina
    performRayTest(frontLeft3Position, frontLeftRayEnd3, iac.rayVertLeft.distance, iac.rayVertLeft.rayCollision, iac.rayVertLeft.type, 3); //lateral

    
    lessDistanceRays(iac.rayFrontLeft, iac.rayDiaLeft, iac.rayVertLeft, e, 0);
    

    // Rayos para la esquina delantera derecha
    performRayTest(frontRight1Position, frontRightRayEnd, iac.rayFrontRight.distance, iac.rayFrontRight.rayCollision, iac.rayFrontRight.type, 4); //recto
    performRayTest(frontRight2Position, frontRightRayEnd2, iac.rayDiaRight.distance, iac.rayDiaRight.rayCollision, iac.rayDiaRight.type, 2); //esquina
    performRayTest(frontRight3Position, frontRightRayEnd3, iac.rayVertRight.distance, iac.rayVertRight.rayCollision, iac.rayVertRight.type, 3); //lateral
    
    
    lessDistanceRays(iac.rayFrontRight, iac.rayDiaRight, iac.rayVertRight, e, 1);
   
    
    // Rayos para la esquina trasera izquierda
    performRayTest(rearLeftPosition, rearLeftRayEnd, iac.rayBackLeft.distance, iac.rayBackLeft.rayCollision, iac.rayBackLeft.type, 5); //recto
    performRayTest(rearLeftPosition, rearLeftRayEnd2, iac.rayBackDia.distance, iac.rayBackDia.rayCollision, iac.rayBackDia.type, 2); //diagonal
    performRayTest(rearLeftPosition, rearLeftRayEnd3, iac.rayBackVert.distance, iac.rayBackVert.rayCollision, iac.rayBackVert.type, 3); //vertical

    
    lessDistanceRays(iac.rayBackLeft, iac.rayBackDia, iac.rayBackVert, e, 2);
    

    // Rayos para la esquina trasera derecha
    performRayTest(rearRightPosition, rearRightRayEnd, iac.rayBackRight.distance, iac.rayBackRight.rayCollision, iac.rayBackRight.type, 6); //recto
    performRayTest(rearRightPosition, rearRightRayEnd2, iac.rayBackDiaRight.distance, iac.rayBackDiaRight.rayCollision, iac.rayBackDiaRight.type, 2); //diagonal
    performRayTest(rearRightPosition, rearRightRayEnd3, iac.rayBackVertRight.distance, iac.rayBackVertRight.rayCollision, iac.rayBackVertRight.type, 3); //vertical

    
    lessDistanceRays(iac.rayBackRight, iac.rayBackDiaRight, iac.rayBackVertRight, e, 3);
    

    performRayTest(frontLeftGround, frontLeftGroundRayEnd, iac.rayGroundFrontLeft.distance, iac.rayGroundFrontLeft.rayCollision, iac.rayGroundFrontLeft.type, 7);
    performRayTest(frontRigthGround, frontRightGroundRayEnd, iac.rayGroundFrontRight.distance, iac.rayGroundFrontRight.rayCollision, iac.rayGroundFrontRight.type, 8);

    lessDistanceRays(iac.rayGroundFrontLeft, iac.rayGroundFrontRight, iac.rayGroundFrontTEST, e, 4);

 

    lessDistance(e, true);

   
}

void AISystem::lessDistance(E &e, bool front)
{
    auto &aiComp = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    if (front == true)
    {
        if (aiComp.targetDistanceTOPLEFT <= aiComp.targetDistanceTOPRIGHT)
        {
            aiComp.targetCollision = aiComp.targetCollisionTOPLEFT;
        }
        else
        {
            aiComp.targetCollision = aiComp.targetCollisionTOPRIGHT;
        }

        if (aiComp.targetTypeGroundL == 4 || aiComp.targetTypeGroundL == 3)
        {
            aiComp.targetCollisionGround = aiComp.targetCollisionGroundL;
        }
        else if(aiComp.targetTypeGroundR == 4 || aiComp.targetTypeGroundR == 3)
        {
            aiComp.targetCollisionGround = aiComp.targetCollisionGroundR;
        }else{
            aiComp.targetCollisionGround = {0.0f, 0.0f, 0.0f};
        }     
    }
    else
    {
        if (aiComp.targetDistanceREARLEFT <= aiComp.targetDistanceREARRIGHT)
        {
            aiComp.targetCollision = aiComp.targetCollisionREARLEFT;
        }
        else
        {
            aiComp.targetCollision = aiComp.targetCollisionREARRIGHT;
        }
    }

}

void AISystem::lessDistanceRays(rays &r1, rays &r2, rays &r3, E &e, int pos)
{
    AIComponent& ai = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    auto selectClosestRay = [&](rays& a, rays& b, rays& c) -> rays& {
        return (a.distance <= b.distance && a.distance <= c.distance) ? a :
               (b.distance <= a.distance && b.distance <= c.distance) ? b : c;
    };

    rays& closest = selectClosestRay(r1, r2, r3);

    switch (pos)
    {
    case 0: // ARRIBA IZQUIERDA
        ai.targetCollisionTOPLEFT = closest.rayCollision;
        ai.targetTypeTOPLEFT = closest.type;
        ai.targetDistanceTOPLEFT = closest.distance;
        break;

    case 1: // ARRIBA DERECHA
        ai.targetCollisionTOPRIGHT = closest.rayCollision;
        ai.targetTypeTOPRIGHT = closest.type;
        ai.targetDistanceTOPRIGHT = closest.distance;
        break;

    case 2: // ABAJO IZQUIERDA
        ai.targetCollisionREARLEFT = closest.rayCollision;
        ai.targetTypeREARLEFT = closest.type;
        ai.targetDistanceREARLEFT = closest.distance;
        break;

    case 3: // ABAJO DERECHA
        ai.targetCollisionREARRIGHT = closest.rayCollision;
        ai.targetTypeREARRIGHT = closest.type;
        ai.targetDistanceREARRIGHT = closest.distance;
        break;

    case 4: // DELANTE SUELO
        ai.targetCollisionGroundL = r1.rayCollision;
        ai.targetTypeGroundL = r1.type;
        ai.targetDistanceGroundL = r1.distance;

        ai.targetCollisionGroundR = r2.rayCollision;
        ai.targetTypeGroundR = r2.type;
        ai.targetDistanceGroundR = r2.distance;
        break;
    }
}


void AISystem::updateScoreActions(E &v)
{
    auto &aiComp = v.getParent().getComponent<AIComponent>(v.getComponentKey<AIComponent>().value());

    float HScore = 0.0f;
    float NScore = 0.0f;
    int index = 0;
    int bindex = 0;

    for (auto &action : aiComp.actions)
    {
        NScore = action->scoreAction(v);
        
        
        // Obtener el nombre de la acción
        std::string actionName = typeid(*action).name();

        
        if (NScore > HScore)
        {
            HScore = NScore;
            bindex = index;
        }

        index++;
    }

    aiComp.actionIt = bindex;
}

void AISystem::checkWaypoint(E& e){

auto &aiComp = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aiwaipoint = v.waypointSiguiente;
    auto &ID_wayp = aiComp.waypoint2Go;
    
    if (aiwaipoint > ID_wayp)
    {
        aiComp.waypoint2Go = aiwaipoint;
        calculateClosestCoord(e);
        aiComp.recalculatePoint = false;
    }
    
}

void AISystem::calculateClosestCoord(E& e)
{
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aiComp = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    int ID_wayp = aiComp.waypoint2Go;
    auto &puntos = e.getParent().getEntitiesAux2();
    auto &coordenadasVehiculo = v.m_carChassis->getWorldTransform().getOrigin();

    auto* waypointEntity = e.getParent().getEntityByPos(puntos[ID_wayp]);
    auto &waypointComponent = e.getParent().getComponent<WaypointComponent>(waypointEntity->getComponentKey<WaypointComponent>().value());

    btRigidBody* waypointBody = waypointComponent.getRigidBody();
    btTransform waypTransform = waypointBody->getWorldTransform();
    btVector3 waypCenter = waypTransform.getOrigin(); // Centro del waypoint

    btBoxShape* boxShape = static_cast<btBoxShape*>(waypointBody->getCollisionShape());
    btVector3 halfExtents = boxShape->getHalfExtentsWithoutMargin();

    if (halfExtents.getZ() < 5) {
        halfExtents.setZ(5);
    }

    btQuaternion rotation = waypTransform.getRotation();
    btMatrix3x3 rotationMatrix(rotation);

    btVector3 corners[4] = {
        btVector3(-halfExtents.getX(), 0, -halfExtents.getZ()),  // Esquina inferior izquierda
        btVector3( halfExtents.getX(), 0, -halfExtents.getZ()),  // Esquina inferior derecha
        btVector3(-halfExtents.getX(), 0,  halfExtents.getZ()),  // Esquina superior izquierda
        btVector3( halfExtents.getX(), 0,  halfExtents.getZ())   // Esquina superior derecha
    };

    btVector3 worldCorners[4];
    for (int i = 0; i < 4; i++) {
        worldCorners[i] = waypCenter + rotationMatrix * corners[i];
    }

    float arrivalRadius = aiComp.arrivalRadius;

    //Logica de cercania a punto
    float posVX = coordenadasVehiculo.getX();
    float posVZ = coordenadasVehiculo.getZ();

    btVector3 vehiclePos(posVX, 0, posVZ);

    int closestIndex = 0;
    float minDistanceSquared = (worldCorners[0] - vehiclePos).length2();

    for (int i = 1; i < 4; ++i) {
        float distanceSquared = (worldCorners[i] - vehiclePos).length2();
        if (distanceSquared < minDistanceSquared) {
            minDistanceSquared = distanceSquared;
            closestIndex = i;
        }
    }

    btVector3 bestPoint;
    minDistanceSquared = FLT_MAX;

    // Función auxiliar para proyectar un punto sobre un segmento
    auto projectPointOnSegment = [](const btVector3& a, const btVector3& b, const btVector3& p) -> btVector3 {
        btVector3 ab = b - a;
        btVector3 ap = p - a;

        float abLengthSquared = ab.length2();
        if (abLengthSquared == 0.0f) return a; // Segmento degenerado

        float t = ap.dot(ab) / abLengthSquared;
        t = std::clamp(t, 0.0f, 1.0f); // Clampear para que se quede dentro del segmento

        return a + t * ab;
    };

    // Definimos los pares de bordes por cada esquina
    const std::array<std::pair<int, int>, 8> edgePairs = {{
        {0, 1}, {0, 2}, // Para corner 0
        {1, 0}, {1, 3}, // Para corner 1
        {2, 0}, {2, 3}, // Para corner 2
        {3, 2}, {3, 1}  // Para corner 3
    }};

    // Calculamos los índices base según la esquina más cercana
    int edgeIndexBase = closestIndex * 2;

    // Primero buscamos el punto más cercano al vehículo en los bordes
    for (int i = 0; i < 2; ++i) {
        const auto& [cornerA, cornerB] = edgePairs[edgeIndexBase + i];

        btVector3 projPoint = projectPointOnSegment(worldCorners[cornerA], worldCorners[cornerB], vehiclePos);
        float distanceSquared = (projPoint - vehiclePos).length2();

        if (distanceSquared < minDistanceSquared) {
            bestPoint = projPoint;
            minDistanceSquared = distanceSquared;
        }
    }

    // Ahora proyectamos la dirección del vehículo sobre el waypoint
    btVector3 vehicleForward = v.m_vehicle->getForwardVector();
    vehicleForward.setY(0); // Asegurarse de que el vector esté en el plano XZ
    vehicleForward.normalize();

    // Creamos un punto muy por delante del coche en su dirección de avance
    btVector3 projectedPoint = vehiclePos + vehicleForward * btSqrt(minDistanceSquared);

    // Proyectamos ese punto sobre los bordes del waypoint
    btVector3 forwardProjection {};
    minDistanceSquared = FLT_MAX;

    for (int i = 0; i < 2; ++i) {
        const auto& [cornerA, cornerB] = edgePairs[edgeIndexBase + i];

        btVector3 projPoint = projectPointOnSegment(worldCorners[cornerA], worldCorners[cornerB], projectedPoint);
        float distanceSquared = (projPoint - projectedPoint).length2();

        if (distanceSquared < minDistanceSquared) {
            forwardProjection = projPoint;
            minDistanceSquared = distanceSquared;
        }
    }

    // Finalmente calculamos el punto medio entre el más cercano y la proyección hacia adelante
    float speed = v.m_vehicle->getCurrentSpeedKmHour() / 3.6f; // O como midas la velocidad
    float weightForward = std::clamp(speed / v.maxSpeed, 0.1f, 0.7f); // Escala dinámica
    float weightBest = 1.0f - weightForward;
    
    btVector3 targetPoint = bestPoint * weightBest + forwardProjection * weightForward;

    aiComp.waypointX = targetPoint.getX();
    aiComp.waypointZ = targetPoint.getZ();

    /*
    


    //Actualizo el cuadrado debug
    
    // auto &renderShape = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
    // renderShape.shape.position = {targetPoint.getX(), 2.0f, targetPoint.getZ()};
    */
     
}
