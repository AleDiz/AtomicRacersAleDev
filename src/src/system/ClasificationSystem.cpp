#include "ClasificationSystem.hpp"
#include "../Entity/Entity.hpp"                // Entidades
#include "../man/EntityManager.hpp"
#include "../components/VehicleComponent.hpp"
#include "../components/WaypointComponent.hpp"

void ClasificationSystem::update(EManager& EM){
    
    auto clasi = EM.getEntitiesAux1();
    auto& control = EM.getEntitiesAux2();

    std::sort(clasi.begin(), clasi.end(), [&](int a, int b) {
        return EM.getComponent<VehicleComponent>(EM.getEntityByPos(a)->getComponentKey<VehicleComponent>().value()).posicion 
               < 
               EM.getComponent<VehicleComponent>(EM.getEntityByPos(b)->getComponentKey<VehicleComponent>().value()).posicion;
    });

    for (int i = 0; i < clasi.size() - 1; i++)
    {
        auto& e1 = EM.getComponent<VehicleComponent>(EM.getEntityByPos(clasi[i])->getComponentKey<VehicleComponent>().value());
        auto& e2 = EM.getComponent<VehicleComponent>(EM.getEntityByPos(clasi[i+1])->getComponentKey<VehicleComponent>().value());

        if(e1.vueltas < e2.vueltas){
            intercambiarPosicion(clasi, i, i+1);

        }else if(e1.vueltas == e2.vueltas){

            if(e1.waypointSiguiente < e2.waypointSiguiente){
                intercambiarPosicion(clasi, i, i+1);

            }else if(e1.waypointSiguiente == e2.waypointSiguiente){
                
                auto& comp1 = EM.getComponent<WaypointComponent>(EM.getEntityByPos(control[e1.waypointSiguiente])->getComponentKey<WaypointComponent>().value());
                auto& comp2 = EM.getComponent<WaypointComponent>(EM.getEntityByPos(control[e2.waypointSiguiente])->getComponentKey<WaypointComponent>().value());

                auto diste1 = (e1.m_carChassis->getCenterOfMassPosition() - 
                            comp1.rigidBody->getCenterOfMassPosition()).length();
                auto diste2 = (e2.m_carChassis->getCenterOfMassPosition() - 
                            comp2.rigidBody->getCenterOfMassPosition()).length();


                if(diste1 >= diste2){
                    intercambiarPosicion(clasi, i, i+1);
                }
            } 
        }
    }

    for (int i = 0; i < clasi.size(); i++)
    {
        auto& e1 = EM.getComponent<VehicleComponent>(EM.getEntityByPos(clasi[i])->getComponentKey<VehicleComponent>().value());

        e1.posicion = i+1;
    }
}


void ClasificationSystem::intercambiarPosicion(std::vector<int>& clasi, int pos1, int pos2){
    std::swap(clasi[pos1], clasi[pos2]);
}

void ClasificationSystem::intercambiarPosicion(VehicleComponent& e1, VehicleComponent& e2){
    int pos1 = e1.posicion;
    int pos2 = e2.posicion;
    e1.posicion = pos2;
    e2.posicion = pos1; 
}