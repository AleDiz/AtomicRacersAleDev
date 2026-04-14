#include "CameraSystem.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../components/CameraComponent.hpp"
#include "../components/VehicleComponent.hpp"
#include "../components/InputComponent.hpp"

#include "../util/EnumsActions.hpp"

void CameraSystem::update(EManager& EM, float deltaTime)
{
    EM.forAllCondition<void(*)(E&, float),CameraComponent>(update_one_entity_camera, deltaTime);
}

void CameraSystem::update_one_entity_camera(E& e, float deltaTime) 
{
    auto& c = e.getParent().getComponent<CameraComponent>(e.getComponentKey<CameraComponent>().value());
    auto& p = e.getParent().getComponent<VehicleComponent>(e.getParent().getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value());

    auto& player_chassis = p.m_carChassis;

    if(c.camType == CameraType::FOLLOW){
        auto& i = e.getParent().getComponent<InputComponent>(e.getParent().getEntityByType(EntityType::PLAYER)->getComponentKey<InputComponent>().value());
        if(c.cinematicIt > 3){
            if(i.actionInput.inputMask & GameActions::ACTION_BACKCAM)
            {
                updateTargetCameraReverse(e, player_chassis, c);
                //updateDebugCamera(e, player_chassis, c);
            }else
            {
                calculateDinamicCamera(e, player_chassis, c, deltaTime);
                updateTargetCamera(e, player_chassis, c);
            } 
        }else{
            if(c.cinematicRestart){
                c.cinematicStartTime = std::chrono::high_resolution_clock::now();
                c.cinematicRestart = false;
            }
            updateCinematic(c);
        }
    }else if(c.camType == CameraType::ORBITAL){
        // Obtener posición y rotación del vehículo
        btVector3 vehiclePosition = p.interpolatedTransform.getOrigin();
        btMatrix3x3 vehicleRotation = p.interpolatedTransform.getBasis();

        myVector3 target = {vehiclePosition.getX(), vehiclePosition.getY() + 1, vehiclePosition.getZ()};
        c.target = target;

        // FOV y distancia fijos
        c.fovy = FOVCAMERA;
        float camDisOffset = DISTANCECAMERA;

        // Ángulo de órbita (actualizar con el tiempo)
        static float orbitAngle = 0.0f;
        orbitAngle += 0.01f; // Ajusta la velocidad de la órbita según necesites

        // Calcular desplazamiento en la órbita
        float offsetX = camDisOffset * cos(orbitAngle);
        float offsetZ = camDisOffset * sin(orbitAngle);
        float offsetY = HEIGHTABOVECAMERA; // Mantener la altura constante

        btVector3 camPosition = vehiclePosition + vehicleRotation * btVector3(offsetX, offsetY, offsetZ);

        myVector3 position = {camPosition.getX(), camPosition.getY(), camPosition.getZ()};
        c.pos = position;

        // Mantener el eje "up" de la cámara
        myVector3 up = {0.0f, 1.0f, 0.0f};
        c.up = up;
    }
    
}

void CameraSystem::calculateDinamicCamera(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c, float deltaTime){
    auto currentVel = player_chassis->getLinearVelocity().length();
    // Parámetros base de la cámara
    float baseDistance = DISTANCECAMERA; // Distancia mínima
    float maxDistance = DISTANCECAMERA+1.0f;  // Distancia máxima sin boost
    float baseFov = FOVCAMERA;     // FOV mínimo
    float maxFov = FOVCAMERA+10.0f;      // FOV máximo sin boost

    // Rango de velocidad en el que se aplicará la interpolación
    float maxSpeed = 42.0f;  // Velocidad máxima (ajústala según tu juego)

    // Factor adicional por boost
    float extraBoostFactor = (currentVel > 26.5f) ? (currentVel - 26.5f) / (maxSpeed - 26.5f) : 0.0f;

    // Aumenta el máximo si hay boost
    float maxDistanceBoosted = maxDistance + extraBoostFactor * 1.5f; // Antes era 2.0, ahora más suave
    float maxFovBoosted = maxFov + extraBoostFactor * 7.0f; // Antes era 10.0, ahora más suave

    // Suavizar la interpolación con una función exponencial o sigmoidea
    float smoothFactor = std::pow(currentVel / maxSpeed, 1.5f);

    // Calcular destino de la cámara (pero no aplicarlo directamente)
    float targetDistance = baseDistance + (maxDistanceBoosted - baseDistance) * smoothFactor;
    float targetFov = baseFov + (maxFovBoosted - baseFov) * smoothFactor;

    // Interpolación suave (lerp)
    float interpolationSpeed = 5.0f * deltaTime; // deltaTime debe venir del motor de juego WARNING RAYLIB
    c.smoothedDistance = c.smoothedDistance + (targetDistance - c.smoothedDistance) * interpolationSpeed;
    c.smoothedFov = c.smoothedFov + (targetFov - c.smoothedFov) * interpolationSpeed;

    // Clamping para evitar valores fuera de rango
    c.smoothedDistance = std::clamp(c.smoothedDistance, baseDistance, maxDistanceBoosted);
    c.smoothedFov = std::clamp(c.smoothedFov, baseFov, maxFovBoosted);
}

void CameraSystem::updateTargetCamera(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c)
{
    auto& p = e.getParent().getComponent<VehicleComponent>(e.getParent().getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value());

    btVector3 vehiclePosition =   p.interpolatedTransform.getOrigin();
    btMatrix3x3 vehicleRotation = p.interpolatedTransform.getBasis();

    myVector3 target = {vehiclePosition.getX(), vehiclePosition.getY(), vehiclePosition.getZ()};

    c.target = target;

    c.fovy = (c.smoothedFov > FOVCAMERA) ? c.smoothedFov : FOVCAMERA;

    float camDisOffset = (c.smoothedDistance > DISTANCECAMERA) ? c.smoothedDistance : DISTANCECAMERA;

    btVector3 offset = vehicleRotation * btVector3(0, HEIGHTABOVECAMERA, -camDisOffset);    //Offset para vista 3ra persona
    // btVector3 offset = vehicleRotation * btVector3(0, HEIGHTABOVECAMERA*20, 0);            //Offset para vista aerea
    btVector3 camPosition = vehiclePosition + offset;

    myVector3 position = {camPosition.getX(), camPosition.getY()  , camPosition.getZ()};
    c.pos = position;

    myVector3 up = {0.0f, 1.0f, 0.0f};              //Up para vista 3ra persona
    // myVector3 up = {0.0f, 0.0f, 1.0f};              //Up para vista aerea

    c.up = up;
}

void CameraSystem::updateTargetCameraReverse(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c)
{
    auto& p = e.getParent().getComponent<VehicleComponent>(e.getParent().getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value());
    
    btVector3 vehiclePosition =   p.interpolatedTransform.getOrigin();
    btMatrix3x3 vehicleRotation = p.interpolatedTransform.getBasis();

    btVector3 offset = vehicleRotation * btVector3(0, HEIGHTABOVECAMERA, -DISTANCECAMERA);    //Offset para vista 3ra persona
    // btVector3 offset = vehicleRotation * btVector3(0, HEIGHTABOVECAMERA*20, 0);            //Offset para vista aerea
    btVector3 camPosition = vehiclePosition + offset;

    myVector3 position = {camPosition.getX(), camPosition.getY()  , camPosition.getZ()};
    c.pos = position;

    btVector3 reverseTargetOffset = vehicleRotation * btVector3(0, 2, -DISTANCECAMERA * 2); // Más lejos hacia atrás
    btVector3 reverseTarget = vehiclePosition + reverseTargetOffset;


    myVector3 target = {reverseTarget.getX(), reverseTarget.getY(), reverseTarget.getZ()};
    c.target = target;


    myVector3 up = {0.0f, 1.0f, 0.0f};
    c.up = up;
}

void CameraSystem::updateCinematic(CameraComponent& c){
    uint8_t posAtual = (c.cinematicIt - 1) * 4;

    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsedTime = std::chrono::duration<float>(currentTime - c.cinematicStartTime).count();

    if (elapsedTime >= 3.0f) {
        c.pos = c.cinematicPos[posAtual + 2];
        c.target = c.cinematicPos[posAtual + 3];
        c.cinematicIt++;
        c.cinematicRestart = true;
        return;
    }

    float t = elapsedTime / 3.0f; // Normalizamos el tiempo entre 0 y 1
    t = std::clamp(t, 0.0f, 1.0f); // Nos aseguramos de que esté en el rango válido
    myVector3 startPos = c.cinematicPos[posAtual];
    myVector3 endPos = c.cinematicPos[posAtual + 2];
    myVector3 startTarget = c.cinematicPos[posAtual + 1];
    myVector3 endTarget = c.cinematicPos[posAtual + 3];
    
    // Interpolamos la posición y el objetivo
    myVector3 interpolatedPos = {
        startPos.x + t * (endPos.x - startPos.x),
        startPos.y + t * (endPos.y - startPos.y),
        startPos.z + t * (endPos.z - startPos.z)
    };
    
    myVector3 interpolatedTarget = {
        startTarget.x + t * (endTarget.x - startTarget.x),
        startTarget.y + t * (endTarget.y - startTarget.y),
        startTarget.z + t * (endTarget.z - startTarget.z)
    };

    c.pos = interpolatedPos;
    c.target = interpolatedTarget;

}

void CameraSystem::updateDebugCamera(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c)
{
    auto& p = e.getParent().getComponent<VehicleComponent>(e.getParent().getEntityByType(EntityType::IA)->getComponentKey<VehicleComponent>().value());

    btVector3 vehiclePosition =   p.interpolatedTransform.getOrigin();
    btMatrix3x3 vehicleRotation = p.interpolatedTransform.getBasis();

    myVector3 target = {vehiclePosition.getX(), vehiclePosition.getY(), vehiclePosition.getZ()};

    c.target = target;

    c.fovy = (c.smoothedFov > FOVCAMERA) ? c.smoothedFov : FOVCAMERA;

    float camDisOffset = (c.smoothedDistance > DISTANCECAMERA) ? c.smoothedDistance : DISTANCECAMERA;

    // btVector3 offset = vehicleRotation * btVector3(0, HEIGHTABOVECAMERA, -camDisOffset);    //Offset para vista 3ra persona
    btVector3 offset = vehicleRotation * btVector3(0, HEIGHTABOVECAMERA*20, 0);            //Offset para vista aerea
    btVector3 camPosition = vehiclePosition + offset;

    myVector3 position = {camPosition.getX(), camPosition.getY()  , camPosition.getZ()};
    c.pos = position;

    // myVector3 up = {0.0f, 1.0f, 0.0f};              //Up para vista 3ra persona
    myVector3 up = {0.0f, 0.0f, 1.0f};              //Up para vista aerea

    c.up = up;
}

