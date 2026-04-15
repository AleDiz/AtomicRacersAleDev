#include "AISystem.hpp"

#include <numbers>
#include <cmath>
#include <random>

#include "../util/EnumsActions.hpp"

#include "../Entity/Entity.hpp"                // Entities
#include "../man/EntityManager.hpp"
#include "../components/VehicleComponent.hpp"
#include "../components/WaypointComponent.hpp"

const float TURN_THRESHOLD = 0.08f; // Threshold in radians (adjustable)

void AISystem::arrive(E &e, float tx, float tz)
{
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &aic = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    btTransform transform = v.m_carChassis->getWorldTransform();
    btVector3 position = transform.getOrigin();

    // Position differences
    float dx = tx - position.getX();
    float dz = tz - position.getZ();
    float distanceSq = dx * dx + dz * dz;

    // Check if we are within arrival radius
    
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
        forwardXZ.setY(0); // Ensure the vector is in the XZ plane
        forwardXZ.normalize();

        if (desiredAngle < 0)
            desiredAngle += static_cast<float>(2.0f * M_PI);

        float currentAngle = std::atan2(forwardXZ.getZ(), forwardXZ.getX());

        // Angular difference (adjust to range [-PI, PI])
        float angleDiff = desiredAngle - currentAngle;
        if (angleDiff > M_PI)
            angleDiff -= static_cast<float>(2.0f * M_PI);
        if (angleDiff < -M_PI)
            angleDiff += static_cast<float>(2.0f * M_PI);

        // Apply turning logic only if angular difference exceeds threshold
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

    // Direction towards the target (seek) or opposite direction (flee)
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

    // Angular difference (adjust to range [-PI, PI])
    float angleDiff = desiredAngle - currentAngle;
    if (angleDiff > M_PI)
        angleDiff -= static_cast<float>(2.0f * M_PI);
    if (angleDiff < -M_PI)
        angleDiff += static_cast<float>(2.0f * M_PI);

    // Movement towards/away from target
    aic.actionInput.inputMask |= GameActions::ACTION_ACCELERATE;
    aic.actionInput.R2 = 0.0f;

    // Turning logic
    if (angleDiff < 0) {
        aic.actionInput.inputMask |= GameActions::ACTION_LEFT;
        aic.actionInput.LJ = -std::clamp(std::abs(angleDiff) / v.max_steering_normal, 0.0f, 1.0f);
    } else if (angleDiff > 0) {
        aic.actionInput.inputMask |= GameActions::ACTION_RIGHT;
        aic.actionInput.LJ = std::clamp(angleDiff / v.max_steering_normal, 0.0f, 1.0f);
    }

    // Drift if angle is steep enough
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

    // Ensure both angles are in [0, 2π]
    if (desiredAngle < 0)
        desiredAngle += static_cast<float>(2.0f * M_PI);
    if (currentAngle < 0)
        currentAngle += static_cast<float>(2.0f * M_PI);

    // Angular difference adjusted to [-PI, PI]
    float angleDiff = desiredAngle - currentAngle;
    if (angleDiff > M_PI)
        angleDiff -= static_cast<float>(2.0f * M_PI);
    if (angleDiff < -M_PI)
        angleDiff += static_cast<float>(2.0f * M_PI);

    // If angle is small, no need to turn
    if (std::abs(angleDiff) < 0.05f) {
        aic.actionInput.LJ = 0.0f;
        return;
    }

    // Apply turning direction
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
    

    // Time refresh
    aiComp.accumulatedTime += deltatime;
    if (aiComp.accumulatedTime >= aiComp.time2Update)
    {
        updateScoreActions(e);
        aiComp.accumulatedTime = 0.0f;
    }

    // Reload power-up cooldown
    aiComp.powerupCooldown -= deltatime;
    if(aiComp.powerupCooldown < 0.0f){
        aiComp.powerupCooldown = 0.0f;
    }

    // Check waypoints
    checkWaypoint(e);
    
    
    if (aiComp.tactive && !aiComp.goingBack)
    {
    // LOGIC WITH UTILITY (NEED TO IMPLEMENT EXECUTE FOR EACH ACTION WITH WHAT THEY SHOULD DO)
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
                    // Align to target
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
        // Logic to return to the track
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
        }
    }
}

void AISystem::update(EManager &EM, btDynamicsWorld *dynamicsWorld, bool updateIA, float deltatime)
{

    EM.forAllCondition<void(*)(E&, btDynamicsWorld*, bool, float), AIComponent>(update_one_entity, dynamicsWorld, updateIA, deltatime);

}

btVector3 AISystem::PredictOvertakePointFromVel(
    const btVector3& myPos,
    const btVector3& rivalPos,     // point detected by raycast (not the rival car's center)
    const btVector3& myReducedVel, // own reduced velocity
    const btVector3& targetWaypoint, 
    float t,
    float lateralDistance,
    int rayLeft,
    int rayRight
) {
    // 1. Future position from own reduced velocity
    btVector3 futurePos = rivalPos + myReducedVel * t;

    // 2. Direction towards waypoint (from my current position)
    btVector3 dirToWaypoint = targetWaypoint - myPos;
    dirToWaypoint.setY(0);
    if (dirToWaypoint.length2() < 0.001f)
        return futurePos;

    dirToWaypoint.normalize();

    // 3. Lateral vector (left relative to direction)
    btVector3 lateral = dirToWaypoint.cross(btVector3(0, 1, 0)).normalized();

    // 4. Determine if waypoint is to the left or right of collision point
    btVector3 toWaypointFromRival = targetWaypoint - rivalPos;
    toWaypointFromRival.setY(0);

    float sideSign = lateral.dot(toWaypointFromRival) > 0 ? 1.0f : -1.0f;

    // 5. Create overtake point on waypoint's side
    btVector3 overtakePoint = futurePos + lateral * (sideSign * lateralDistance);

    return overtakePoint;
}



void AISystem::detectCollisions(btDynamicsWorld *dynamicsWorld, E &e)
{
    // Get current car transform
    auto &v = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto &iac = e.getParent().getComponent<AIComponent>(e.getComponentKey<AIComponent>().value());

    auto carTransform = v.m_carChassis->getWorldTransform();
  

    // Ray length
    float rayLength = iac.visionDistance;

    // Define offsets for car corners (front and rear)
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

    // Calculate global positions for corners
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

    // Diagonal directions for rays
    // Front left
    btVector3 diagonalDirectionFL1 = carTransform.getBasis() * btVector3(0,0,1);  
    btVector3 diagonalDirectionFL2 = carTransform.getBasis() * btVector3(0.577f, 0, 1);  // Front left
    btVector3 diagonalDirectionFL3 = carTransform.getBasis() * btVector3(0.577f, 0, 1);

    // Front right
    btVector3 diagonalDirectionFR1 = carTransform.getBasis() * btVector3(0,0,1);
    btVector3 diagonalDirectionFR2 = carTransform.getBasis() * btVector3(-0.577f, 0, 1); 
    btVector3 diagonalDirectionFR3 = carTransform.getBasis() * btVector3(-0.577f, 0, 1);


    // Rear left
    btVector3 diagonalDirectionRL1 = carTransform.getBasis() * btVector3(0,0,-1);
    btVector3 diagonalDirectionRL2 = carTransform.getBasis() * btVector3(1, 0, -1);
    btVector3 diagonalDirectionRL3 = carTransform.getBasis() * btVector3(1, 0, 0);

    // Rear right
    btVector3 diagonalDirectionRR1 = carTransform.getBasis() * btVector3(0,0,-1);
    btVector3 diagonalDirectionRR2 = carTransform.getBasis() * btVector3(-1, 0, -1);
    btVector3 diagonalDirectionRR3 = carTransform.getBasis() * btVector3(-1, 0, 0);


    // Rays to ground
    btVector3 diagonalDirectionFL_Ground = btVector3(0, -1, 0);
    btVector3 diagonalDirectionFR_Ground = btVector3(0, -1, 0);

    

    // Define rays (start and end)
    // left
    btVector3 frontLeftRayEnd = frontLeft1Position + diagonalDirectionFL1.normalized() * rayLength;
    btVector3 frontLeftRayEnd2 = frontLeft2Position + diagonalDirectionFL2.normalized() * rayLength;
    btVector3 frontLeftRayEnd3 = frontLeft3Position + diagonalDirectionFL3.normalized() * rayLength;

    // right
    btVector3 frontRightRayEnd = frontRight1Position + diagonalDirectionFR1.normalized() * rayLength;
    btVector3 frontRightRayEnd2 = frontRight2Position + diagonalDirectionFR2.normalized() * rayLength;
    btVector3 frontRightRayEnd3 = frontRight3Position + diagonalDirectionFR3.normalized() * rayLength;

    // rear left
    btVector3 rearLeftRayEnd = rearLeftPosition + diagonalDirectionRL1.normalized() * rayLength;
    btVector3 rearLeftRayEnd2 = rearLeftPosition + diagonalDirectionRL2.normalized() * rayLength;
    btVector3 rearLeftRayEnd3 = rearLeftPosition + diagonalDirectionRL3.normalized() * rayLength;

    // rear right
    btVector3 rearRightRayEnd = rearRightPosition + diagonalDirectionRR1.normalized() * rayLength;
    btVector3 rearRightRayEnd2 = rearRightPosition + diagonalDirectionRR2.normalized() * rayLength;
    btVector3 rearRightRayEnd3 = rearRightPosition + diagonalDirectionRR3.normalized() * rayLength;

    // ground rays
    btVector3 frontLeftGroundRayEnd = frontLeftGround + diagonalDirectionFL_Ground.normalized() * rayLength;
    btVector3 frontRightGroundRayEnd = frontRigthGround + diagonalDirectionFR_Ground.normalized() * rayLength;
   
    

    // Lambda to perform ray tests
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


            const char* objectName = "Unknown";  // Default value

            if (collidedEntity != nullptr)
            {
                switch (collidedEntity->tipo)
                {
                    case EntityType::PLAYER:
                    case EntityType::IA:
                        objectName = "Car";
                        targetType = 1;
                        break;
                    case EntityType::OBJETOSMUNDO:
                        objectName = "Wall";
                        targetType = 0;
                        break;
                    case EntityType::POWERUP:
                        objectName = "Power-up";
                        targetType = 2;
                        break;
                    case EntityType::GROUNDINFINITY:
                        objectName = "Slow ground";
                        targetType = 3;
                        break;
                    case EntityType::BOOSTGROUND:
                        objectName = "Boost ground";
                        targetType = 4;
                        break;
                    case EntityType::ROAD:
                        objectName = "Road";
                        targetType = 5;
                        break;
                    default:
                        objectName = "Unknown object";
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

    // Front left rays
    performRayTest(frontLeft1Position, frontLeftRayEnd, iac.rayFrontLeft.distance, iac.rayFrontLeft.rayCollision, iac.rayFrontLeft.type, 1); // straight
    performRayTest(frontLeft2Position, frontLeftRayEnd2, iac.rayDiaLeft.distance, iac.rayDiaLeft.rayCollision, iac.rayDiaLeft.type, 2); // corner
    performRayTest(frontLeft3Position, frontLeftRayEnd3, iac.rayVertLeft.distance, iac.rayVertLeft.rayCollision, iac.rayVertLeft.type, 3); // lateral

    
    lessDistanceRays(iac.rayFrontLeft, iac.rayDiaLeft, iac.rayVertLeft, e, 0);
    

    // Rays for front right corner
    performRayTest(frontRight1Position, frontRightRayEnd, iac.rayFrontRight.distance, iac.rayFrontRight.rayCollision, iac.rayFrontRight.type, 4); // straight
    performRayTest(frontRight2Position, frontRightRayEnd2, iac.rayDiaRight.distance, iac.rayDiaRight.rayCollision, iac.rayDiaRight.type, 2); // corner
    performRayTest(frontRight3Position, frontRightRayEnd3, iac.rayVertRight.distance, iac.rayVertRight.rayCollision, iac.rayVertRight.type, 3); // lateral
    
    
    lessDistanceRays(iac.rayFrontRight, iac.rayDiaRight, iac.rayVertRight, e, 1);
   
    
    // Rays for rear left corner
    performRayTest(rearLeftPosition, rearLeftRayEnd, iac.rayBackLeft.distance, iac.rayBackLeft.rayCollision, iac.rayBackLeft.type, 5); // straight
    performRayTest(rearLeftPosition, rearLeftRayEnd2, iac.rayBackDia.distance, iac.rayBackDia.rayCollision, iac.rayBackDia.type, 2); // diagonal
    performRayTest(rearLeftPosition, rearLeftRayEnd3, iac.rayBackVert.distance, iac.rayBackVert.rayCollision, iac.rayBackVert.type, 3); // vertical

    
    lessDistanceRays(iac.rayBackLeft, iac.rayBackDia, iac.rayBackVert, e, 2);
    

    // Rays for rear right corner
    performRayTest(rearRightPosition, rearRightRayEnd, iac.rayBackRight.distance, iac.rayBackRight.rayCollision, iac.rayBackRight.type, 6); // straight
    performRayTest(rearRightPosition, rearRightRayEnd2, iac.rayBackDiaRight.distance, iac.rayBackDiaRight.rayCollision, iac.rayBackDiaRight.type, 2); // diagonal
    performRayTest(rearRightPosition, rearRightRayEnd3, iac.rayBackVertRight.distance, iac.rayBackVertRight.rayCollision, iac.rayBackVertRight.type, 3); // vertical

    
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
    case 0: // TOP LEFT
        ai.targetCollisionTOPLEFT = closest.rayCollision;
        ai.targetTypeTOPLEFT = closest.type;
        ai.targetDistanceTOPLEFT = closest.distance;
        break;

    case 1: // TOP RIGHT
        ai.targetCollisionTOPRIGHT = closest.rayCollision;
        ai.targetTypeTOPRIGHT = closest.type;
        ai.targetDistanceTOPRIGHT = closest.distance;
        break;

    case 2: // BOTTOM LEFT
        ai.targetCollisionREARLEFT = closest.rayCollision;
        ai.targetTypeREARLEFT = closest.type;
        ai.targetDistanceREARLEFT = closest.distance;
        break;

    case 3: // BOTTOM RIGHT
        ai.targetCollisionREARRIGHT = closest.rayCollision;
        ai.targetTypeREARRIGHT = closest.type;
        ai.targetDistanceREARRIGHT = closest.distance;
        break;

    case 4: // FRONT GROUND
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
        
        
        // Get action name
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
    btVector3 waypCenter = waypTransform.getOrigin(); // Waypoint center

    btBoxShape* boxShape = static_cast<btBoxShape*>(waypointBody->getCollisionShape());
    btVector3 halfExtents = boxShape->getHalfExtentsWithoutMargin();

    if (halfExtents.getZ() < 5) {
        halfExtents.setZ(5);
    }

    btQuaternion rotation = waypTransform.getRotation();
    btMatrix3x3 rotationMatrix(rotation);

    btVector3 corners[4] = {
        btVector3(-halfExtents.getX(), 0, -halfExtents.getZ()),  // Bottom left corner
        btVector3( halfExtents.getX(), 0, -halfExtents.getZ()),  // Bottom right corner
        btVector3(-halfExtents.getX(), 0,  halfExtents.getZ()),  // Top left corner
        btVector3( halfExtents.getX(), 0,  halfExtents.getZ())   // Top right corner
    };

    btVector3 worldCorners[4];
    for (int i = 0; i < 4; i++) {
        worldCorners[i] = waypCenter + rotationMatrix * corners[i];
    }

    float arrivalRadius = aiComp.arrivalRadius;

    // Proximity logic to point
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

    // Helper function to project a point onto a segment
    auto projectPointOnSegment = [](const btVector3& a, const btVector3& b, const btVector3& p) -> btVector3 {
        btVector3 ab = b - a;
        btVector3 ap = p - a;

        float abLengthSquared = ab.length2();
        if (abLengthSquared == 0.0f) return a; // Degenerate segment

        float t = ap.dot(ab) / abLengthSquared;
        t = std::clamp(t, 0.0f, 1.0f); // Clamp to stay within segment

        return a + t * ab;
    };

    // Define edge pairs for each corner
    const std::array<std::pair<int, int>, 8> edgePairs = {{
        {0, 1}, {0, 2}, // For corner 0
        {1, 0}, {1, 3}, // For corner 1
        {2, 0}, {2, 3}, // For corner 2
        {3, 2}, {3, 1}  // For corner 3
    }};

    // Calculate base indices according to closest corner
    int edgeIndexBase = closestIndex * 2;

    // First find closest point to vehicle on edges
    for (int i = 0; i < 2; ++i) {
        const auto& [cornerA, cornerB] = edgePairs[edgeIndexBase + i];

        btVector3 projPoint = projectPointOnSegment(worldCorners[cornerA], worldCorners[cornerB], vehiclePos);
        float distanceSquared = (projPoint - vehiclePos).length2();

        if (distanceSquared < minDistanceSquared) {
            bestPoint = projPoint;
            minDistanceSquared = distanceSquared;
        }
    }

    // Now project vehicle direction onto waypoint
    btVector3 vehicleForward = v.m_vehicle->getForwardVector();
    vehicleForward.setY(0); // Ensure vector is in XZ plane
    vehicleForward.normalize();

    // Create a point far ahead of car in its direction
    btVector3 projectedPoint = vehiclePos + vehicleForward * btSqrt(minDistanceSquared);

    // Project that point onto waypoint edges
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

    // Finally calculate midpoint between closest and forward projection
    float speed = v.m_vehicle->getCurrentSpeedKmHour() / 3.6f; // Or however you measure speed
    float weightForward = std::clamp(speed / v.maxSpeed, 0.1f, 0.7f); // Dynamic scale
    float weightBest = 1.0f - weightForward;
    
    btVector3 targetPoint = bestPoint * weightBest + forwardProjection * weightForward;

    aiComp.waypointX = targetPoint.getX();
    aiComp.waypointZ = targetPoint.getZ();

    /*
    

    // Update debug square
    
    // auto &renderShape = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
    // renderShape.shape.position = {targetPoint.getX(), 2.0f, targetPoint.getZ()};
    */
     
}
