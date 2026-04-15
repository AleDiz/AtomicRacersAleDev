#include "NotAISystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../util/ObjectRigidType.hpp"

void NotAISystem::update(EManager& EM, float deltaTime){
    EM.forAllCondition<void(*)(E&, float), NotAIComponent>(update_one_entity, deltaTime);
};

void NotAISystem::update_one_entity(E& e, float deltaTime){
   auto& comp = e.getParent().getComponent<NotAIComponent>(e.getComponentKey<NotAIComponent>().value());

    const float speed = 2.0f; // unidades por segundo

    myVector3& posActual = comp.posActual;
    const myVector3& target = comp.hacia2 ? comp.pos2 : comp.pos1;

    // Dirección desde la posición actual al destino
    myVector3 direction {
        target.x - posActual.x,
        target.y - posActual.y,
        target.z - posActual.z
    };

    // Calcular distancia al objetivo
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

    // Si ya estamos cerca del objetivo, cambiamos de dirección
    if (distance < 0.1f) { // Umbral ajustable
        comp.hacia2 = !comp.hacia2;

        // Recalcula la dirección hacia el nuevo objetivo
        myVector3 newTarget = comp.hacia2 ? comp.pos2 : comp.pos1;
        myVector3 lookDir {
            newTarget.x - comp.posActual.x,
            newTarget.y - comp.posActual.y,
            newTarget.z - comp.posActual.z
        };

        // Solo interesa la rotación en el plano XZ
        float angleY = std::atan2(lookDir.x, lookDir.z) * (180.0f / M_PI);
        comp.rot.fromAxisAngle({0.0f, 1.0f, 0.0f}, angleY);
        return;
    }

    // Recalcula la dirección hacia el nuevo objetivo
    myVector3 newTarget = comp.hacia2 ? comp.pos2 : comp.pos1;
    myVector3 lookDir {
        newTarget.x - comp.posActual.x,
        newTarget.y - comp.posActual.y,
        newTarget.z - comp.posActual.z
    };

    // Solo interesa la rotación en el plano XZ
    float angleY = std::atan2(lookDir.x, lookDir.z) * (180.0f / M_PI);
    comp.rot.fromAxisAngle({0.0f, 1.0f, 0.0f}, angleY);

    // Normalizamos la dirección y aplicamos movimiento
    direction.normalize();

    posActual.x += direction.x * speed * deltaTime;
    posActual.y += direction.y * speed * deltaTime;
    posActual.z += direction.z * speed * deltaTime;
}