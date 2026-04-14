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

        float   arrivalRadius {1.0f};    // Radio de llegada: Margen que detecta si ha llegado al punto o no /// 10.0
        float   time2arrive   {0.15f};    // Tiempo que espera llegar al punto, acelera cuando esta lejos y empieza a frenar cuando esta a 0.5 /// 0.15
                                          // segundos de llegar l punto.
        float   visionDistance {14.0f};    // Distancia de vision del vehiculo
        

        float   accumulatedTime {0.0f};   // Tiempo desde la ultima actualizacion del comportamiento
        float   time2Update     {0.1f};   // Tiempo que tarda en actualizar el comportamiento. Es el inverso de la frecuencia.
                                          // si quiero percibir 10 veces por segundo es 1/10, 5 veces por segundo 1/5 ó 0.2 ...

        float waypointX {0.0f};
        float waypointZ {0.0f};

        SB      behaviour       {SB::PATHFOLLOWING};
 

        int waypoint2Go {-1}; //Marca el id del waypoint al que tendria que ir
        std::array<std::shared_ptr<Action>, 8> actions;
        int actionIt {0};

        bool recalculatePoint {false};
        float powerupCooldown {0.0f};
        
        myInput actionInput {};
        int initDriftDirection {};

        btVector3 targetCollision{0.0f, 0.0f, 0.0f}; // posicion FINAL
        btVector3 targetCollisionGround{0.0f, 0.0f, 0.0f}; // posicion FINAL


        //collisiones finales
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



        //estructuras que pasar al detect collision
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

        //rayos suelo
        rays rayGroundFrontLeft{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayGroundFrontRight{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};
        rays rayGroundFrontTEST{0.0f, btVector3(20.0f, 0.0f, 0.0f), -1};



        void initAIComponent(float arrivalRadius, float time2arrive, float visionDistance, float time2Update);

        static std::array<std::shared_ptr<Action>, 8> createActions();

        static void findClosestRay(const rays& ray1, const rays& ray2, const rays& ray3);

        void clearComponent();

};

#endif