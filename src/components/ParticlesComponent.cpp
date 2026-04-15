#include "ParticlesComponent.hpp"
#include "../util/JsonManager.hpp"

#include <random>

void ParticlesComponent::initParticle(int cant, myVector3 ori, uint16_t model, int idEm){
    cantidad_ = cant;

    particles_.reserve(cantidad_);
    origin_ = ori;
    idModel = model;
    emitterID = idEm;
}

void ParticlesComponent::init_one_particle(){
    
    switch (type)
    {
    case STAR:
        for (int i = 0; i < cantidad_; i++)
        {
            Particle newParticle;
            
            static std::random_device rd;
            static std::mt19937 gen(rd());

            //POSICION
            newParticle.position_ = origin_;

            //DIRECCION
            std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
            newParticle.direction_ = myVector3(
                dirDist(gen),
                dirDist(gen),
                dirDist(gen)
            ).normalized();

            //VELOCIDAD
            std::uniform_real_distribution<float> speedDist(5.0f, 7.0f);
            newParticle.velocity_ = newParticle.direction_ * speedDist(gen);

            //ANGULO
            std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.141592f);  // Ángulos en radianes [0, 2π]
            std::uniform_real_distribution<float> axisDist(-1.0f, 1.0f);              // Eje aleatorio

            myVector3 randomAxis = myVector3(
                axisDist(gen),
                axisDist(gen),
                axisDist(gen)
            ).normalized();

            float randomAngle = angleDist(gen);

            newParticle.rotation_.fromAxisAngle2(randomAxis, randomAngle);

            //TIEMPO DE VIDA
            std::uniform_real_distribution<float> lifetimeDist(0.1f, 0.2f);
            newParticle.lifetime_ = lifetimeDist(gen);

            particles_.push_back(newParticle);
        }
        break;
    
    case SPARK:
    case CLOUD:
        for (int i = 0; i < cantidad_; i++)
        {
            Particle newParticle;
            
            static std::random_device rd;
            static std::mt19937 gen(rd());

            //POSICION
            newParticle.position_ = origin_;

            //DIRECCION
            std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
            newParticle.direction_ = myVector3(
                dirDist(gen),
                dirDist(gen),
                dirDist(gen)
            ).normalized();

            //VELOCIDAD
            std::uniform_real_distribution<float> speedDist(5.0f, 7.0f);
            newParticle.velocity_ = newParticle.direction_ * speedDist(gen);

            //ANGULO
            std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.141592f);  // Ángulos en radianes [0, 2π]
            std::uniform_real_distribution<float> axisDist(-1.0f, 1.0f);              // Eje aleatorio

            myVector3 randomAxis = myVector3(
                axisDist(gen),
                axisDist(gen),
                axisDist(gen)
            ).normalized();

            float randomAngle = angleDist(gen);

            newParticle.rotation_.fromAxisAngle2(randomAxis, randomAngle);

            //TIEMPO DE VIDA
            std::uniform_real_distribution<float> lifetimeDist(0.1f, 0.2f);
            newParticle.lifetime_ = lifetimeDist(gen);

            particles_.push_back(newParticle);
        }
        break;
    }

}

void ParticlesComponent::initSparkParticle(Particle& p, const myVector3& wheelPos, bool isLeftWheel){
    
    // 1. Posición inicial (rueda + pequeño desplazamiento aleatorio)
    myVector3 wP = myVector3(
        randFloat(-0.2f, 0.2f),
        randFloat(0.0f, 0.1f),
        randFloat(-0.2f, 0.2f)
    );
    p.position_ = wheelPos + wP;

    // 2. Velocidad inicial (dirección basada en el derrape)
    float directionSign = isLeftWheel ? 1.0f : -1.0f; // Sentido contrario por rueda
    p.velocity_ = myVector3(
        directionSign * randFloat(1.0f, 3.0f), // Horizontal (efecto derrape)
        randFloat(1.0f, 3.0f),                 // Vertical (salto inicial)
        randFloat(-0.5f, 0.5f)                 // Profundidad (aleatorio)
    );

    // 3. Tiempo de vida corto (chispas rápidas)
    p.lifetime_ = randFloat(0.3f, 1.0f);

    // 4. Gravedad fuerte (para que caigan rápido)
    p.gravity_ = myVector3(0.0f, -15.0f, 0.0f);
    
}

void ParticlesComponent::clearComponent(){
    particles_.clear();
}
