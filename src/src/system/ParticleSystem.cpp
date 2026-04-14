#include "ParticleSystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"


void ParticleSystem::update(EManager& EM, float deltaTime){
    EM.forAllCondition<void(*)(E&, float), ParticlesComponent>(update_one_entity, deltaTime);
};

void ParticleSystem::update_one_entity(E& e, float deltaTime){
    auto& pc = e.getParent().getComponent<ParticlesComponent>(e.getComponentKey<ParticlesComponent>().value());

    switch (pc.type)
    {
    case STAR:
        for (auto it = pc.particles_.begin(); it != pc.particles_.end(); ) {
            Particle& p = *it;

            p.position_.x += p.velocity_.x * deltaTime;
            p.position_.y += p.velocity_.y * deltaTime;
            p.position_.z += p.velocity_.z * deltaTime;
 

            p.lifetime_ -= deltaTime;
            if (p.lifetime_ <= 0.0f) {
                it = pc.particles_.erase(it);
            } else {
                ++it;
            }

        }
        break;
    
    case CLOUD:
        {
        auto* entityEmitter = e.getParent().getEntityById(e.getParent().getComponent<ParticlesComponent>(e.getComponentKey<ParticlesComponent>().value()).emitterID);
       
        if(entityEmitter == nullptr){
            e.state = false;
            break;
        }

        auto& vc = entityEmitter->getParent().getComponent<VehicleComponent>(entityEmitter->getComponentKey<VehicleComponent>().value());

        myVector3 leftWheelPos = vc.getWheelLeftBackPos();
        myVector3 rightWheelPos = vc.getWheelRightBackPos();

        // Contadores para partículas por rueda
        int particlesPerWheel = pc.particles_.size() / 2;

        // ---- Actualización de partículas ----
        for (auto it = pc.particles_.begin(); it != pc.particles_.end(); ++it) {
            Particle& p = *it;

            if (!p.isActive_) continue; // Saltar partículas inactivas

            // 1. Aplicar gravedad
            p.velocity_.x += p.gravity_.x * deltaTime;
            p.velocity_.y += p.gravity_.y * deltaTime;
            p.velocity_.z += p.gravity_.z * deltaTime;

            // 2. Actualizar posición
            p.position_.x += p.velocity_.x * deltaTime;
            p.position_.y += p.velocity_.y * deltaTime;
            p.position_.z += p.velocity_.z * deltaTime;

            // 3. Reducir tiempo de vida
            p.lifetime_ -= deltaTime;
            if (p.lifetime_ <= 0.0f) {
                p.isActive_ = false; // Marcar para reutilización
            }
        }

        // ---- Generación constante de chispas ----
        for (int i = 0; i < pc.particles_.size(); ++i) {
            Particle& p = pc.particles_[i];

            // Solo reactivar partículas inactivas
            if (p.isActive_) continue;

            // Decidir si es para rueda izquierda o derecha
            bool isLeftWheel = (i < particlesPerWheel);
            myVector3 wheelPos = isLeftWheel ? leftWheelPos : rightWheelPos;

            // Inicializar chispa
            pc.initSparkParticle(p, wheelPos, isLeftWheel);
            p.isActive_ = true;
        }

        if(!vc.drifting){
            e.state = false;
        }
        }
        break;

    case SPARK:
        {
        auto* entityEmitter = e.getParent().getEntityById(e.getParent().getComponent<ParticlesComponent>(e.getComponentKey<ParticlesComponent>().value()).emitterID);
       
        if(entityEmitter == nullptr){
            e.state = false;
            break;
        }

        auto& vc = entityEmitter->getParent().getComponent<VehicleComponent>(entityEmitter->getComponentKey<VehicleComponent>().value());

        if(vc.driftBoost > 1.0f){
            myVector3 leftWheelPos = vc.getWheelLeftBackPos();
            myVector3 rightWheelPos = vc.getWheelRightBackPos();

            // Contadores para partículas por rueda
            int particlesPerWheel = pc.particles_.size() / 2;

            // ---- Actualización de partículas ----
            for (auto it = pc.particles_.begin(); it != pc.particles_.end(); ++it) {
                Particle& p = *it;

                if (!p.isActive_) continue; // Saltar partículas inactivas

                // 1. Aplicar gravedad
                p.velocity_.x += p.gravity_.x * deltaTime;
                p.velocity_.y += p.gravity_.y * deltaTime;
                p.velocity_.z += p.gravity_.z * deltaTime;

                // 2. Actualizar posición
                p.position_.x += p.velocity_.x * deltaTime;
                p.position_.y += p.velocity_.y * deltaTime;
                p.position_.z += p.velocity_.z * deltaTime;

                // 3. Reducir tiempo de vida
                p.lifetime_ -= deltaTime;
                if (p.lifetime_ <= 0.0f) {
                    p.isActive_ = false; // Marcar para reutilización
                }
            }

            // ---- Generación constante de chispas ----
            for (int i = 0; i < pc.particles_.size(); ++i) {
                Particle& p = pc.particles_[i];

                // Solo reactivar partículas inactivas
                if (p.isActive_) continue;

                // Decidir si es para rueda izquierda o derecha
                bool isLeftWheel = (i < particlesPerWheel);
                myVector3 wheelPos = isLeftWheel ? leftWheelPos : rightWheelPos;

                // Inicializar chispa
                pc.initSparkParticle(p, wheelPos, isLeftWheel);
                p.isActive_ = true;
            }
        }
        
        if(!vc.drifting){
            e.state = false;
        }
        }
        break;
    }

    if(pc.particles_.empty()){
        e.state = false;
    }
}