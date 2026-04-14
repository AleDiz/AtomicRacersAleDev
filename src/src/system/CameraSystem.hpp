#pragma once

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "../util/UsingTypes.hpp"
#include <memory>

class btRigidBody;


class CameraSystem
{ 
    public:
        void update(EManager& EM, float deltaTime);
        static void update_one_entity_camera(E& e, float deltaTime);
        static void calculateDinamicCamera(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c, float deltaTime);
        static void updateTargetCameraReverse(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c);
        static void updateTargetCamera(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c);
        static void updateCinematic(CameraComponent& c);

        static void updateDebugCamera(E& e, std::unique_ptr<btRigidBody>& player_chassis, CameraComponent& c);


};

#endif //CAMERASYSTEM_H