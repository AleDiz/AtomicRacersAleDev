#pragma once

#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <cstdint>
#include <array>
#include <memory>

#include "../util/UtilityAI.hpp"
#include "../util/BasicUtils.hpp"
#include "../bullet/btBulletDynamicsCommon.h"

struct rays
{
    float distance;
    btVector3 rayCollision;
    int type;
};

class AIComponent {

    public:
        static constexpr int Capacity{7};


        bool    tactive    { true };
        int     posicion    {0};

        float   arrivalRadius {1.0f};    // Arrival radius: Margin that detects if it has reached the point or not /// 10.0
        float   time2arrive   {0.15f};   // Time expected to reach the point, accelerates when far and starts to brake when at 0.5 /// 0.15
                                          // seconds from reaching the point.
        float   visionDistance {14.0f};  // Vision distance of the vehicle
        

        float   accumulatedTime {0.0f};  // Time since the last behavior update
        float   time2Update     {0.1f};  // Time it takes to update the behavior. It is the inverse of the frequency.
                                          // if I want to perceive 10 times per second it is 1/10, 5 times per second 1/5 or 0.2 ...

        float waypointX {0.0f};
        float waypointZ {0.0f};

        SB      behaviour       {SB::PATHFOLLOWING};
 

        int waypoint2Go {-1}; // Marks the id of the waypoint it should go to
        std::array<std::shared_ptr<Action>, 8> actions;
        int actionIt {0};

        bool recalculatePoint {false};
        float powerupCooldown {0.0f};
        
        myInput actionInput {};
        int initDriftDirection {};

        btVector3 targetCollision{0.0f, 0.0f, 0.0f}; // FINAL position
        btVector3 targetCollisionGround{0.0f, 0.0f, 0.0f}; // FINAL position


        // Final collisions
        float targetDistanceTOPLEFT{20.0f}; 
        float targetDistanceTOPRIGHT{20.0f};
        float targetDistanceREARLEFT{20.0f};
        float targetDistanceREARRIGHT{20.0f};

        float targetDistanceGroundL{20.0f};
        float targetDistanceGroundR{20.0f};

    
        int targetTypeTOPLEFT {-1};
        int targetTypeTOPRIGHT {-1};
        int targetTypeREARLEFT {-1};
        int targetTypeREARRIGHT {-1};

        int targetTypeGroundL {-1};
        int targetTypeGroundR {-1};

        bool goingBack {false};
        float timeOut {0.0f};


        btVector3 targetCollisionTOPLEFT{0.0f, 0.0f, 0.0f}; 
        btVector3 targetCollisionTOPRIGHT{0.0f, 0.0f, 0.0f};
        btVector3 targetCollisionREARLEFT{0.0f, 0.0f, 0.0f};
        btVector3 targetCollisionREARRIGHT{0.0f, 0.0f, 0.0f};

        btVector3 targetCollisionGroundL{0.0f, 0.0f, 0.0f};
        btVector3 targetCollisionGroundR{0.0f, 0.0f, 0.0f};



        // Structures to pass to detectCollision
        rays rayFrontLeft{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayDiaLeft{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayVertLeft{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};

        rays rayFrontRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayDiaRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayVertRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};

        rays rayBackLeft{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayBackDia{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayBackVert{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        
        rays rayBackRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayBackDiaRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayBackVertRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};

        // Ground rays
        rays rayGroundFrontLeft{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayGroundFrontRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayGroundFrontTEST{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};



        void initAIComponent(float arrivalRadius, float time2arrive, float visionDistance, float time2Update);

        static std::array<std::shared_ptr<Action>, 8> createActions();

        static void findClosestRay(const rays& ray1, const rays& ray2, const rays& ray3);

        void clearComponent();

};

#endif