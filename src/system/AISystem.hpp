#pragma once

#ifndef AISYSTEM_H
#define AISYSTEM_H


//#include "../components/blackboard.cpp"
#include <bullet/btBulletDynamicsCommon.h>

#include "../util/UsingTypes.hpp"
#include <iostream>
#include "../components/AIComponent.hpp"

class AISystem{
    
    public:
    //Aqui tienen que ir todas las funciones que se encargan del comportamiento de la IA (seek, pathfollowing, etc)
    static void arrive(E& e, float tx, float tz);
    static void seek(E& e, float tx, float tz, int target);
    static void flee(E& e, float tx, float tz);
    static void align(E& e, float tx, float tz, bool backwards);
    static void update_one_entity(E& e, btDynamicsWorld* dynamicsWorld, bool updateIA, float deltatime);
    void update(EManager& EM, btDynamicsWorld* dynamicsWorld, bool updateIA, float deltatime);
    static void detectCollisions(btDynamicsWorld* dynamicsWorld, E& e);
    static void lessDistance(E& e, bool front);
    static void lessDistanceRays(rays &r1, rays &r2, rays &r3, E& e, int pos);
    static void updateScoreActions(E& v);    
    static void checkWaypoint(E& e);
    static void calculateClosestCoord(E& e);
    static btVector3 PredictOvertakePointFromVel(const btVector3& myPos, const btVector3& rivalPos, const btVector3& myReducedVel, const btVector3& targetWaypoint, float t, float lateralDistance, int rayLeft, int rayRight);
};


#endif