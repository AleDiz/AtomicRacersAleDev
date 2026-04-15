
#pragma once

#ifndef PARTICLESCOMPONENT_H
#define PARTICLESCOMPONENT_H

#include <memory>
#include <vector>
#include <cstdint>
#include <random>

#include "../util/BasicUtils.hpp"
#include "../util/ParticleTypes.hpp"
#include "../util/UsingTypes.hpp"

struct Particle {
    myVector3       position_{};

    myQuaternion    rotation_{};
    myVector3       rotationAxis_{};
    float           angularSpeed{};

    myVector3       direction_{};
    myVector3       velocity_{};
    myVector3       gravity_{0.0f, -9.8f, 0.0f};

    float lifetime_{};
    bool isActive_{false}; // Para reutilización
};

class ParticlesComponent {
public:
    static constexpr int Capacity{30};

    myVector3 origin_{};
    std::vector<Particle> particles_{};

    uint16_t idModel {};
    uint16_t cantidad_{};
    float spawnRate_{};

    ParticleTypes type{};

    int emitterID{};

    void initParticle(int cant, myVector3 ori, uint16_t model, int idEm);
    void init_one_particle();
    void initSparkParticle(Particle& p, const myVector3& wheelPos, bool isLeftWheel);

    void clearComponent();

    float randFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
    
};


#endif