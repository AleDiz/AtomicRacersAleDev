#include "InputSystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../util/EnumsActions.hpp"


#include "../components/InputComponent.hpp"

void InputSystem::update(EManager& EM, myInput actionMask){
        EM.forAllAux1<void(*)(E&, myInput)>(update_one_entity, actionMask);
};

void InputSystem::cleanInput(EManager& EM){
        EM.forAllAux1<void(*)(E&)>(clean_one_entity);
};

void InputSystem::debugCars(EManager& EM){
        EM.forAllAux1<void(*)(E&)>(debug_one_entity);
};

void InputSystem::update_one_entity(E& e, myInput actionMask){

    if(e.getParent().hasComponent<InputComponent>(e)) {
        if(e.tipo == EntityType::PLAYER){
            auto& in = e.getParent().getComponent<InputComponent>(e.getComponentKey<InputComponent>().value());

            in.actionInput.inputMask = actionMask.inputMask;
            in.actionInput.R2 = actionMask.R2;
            in.actionInput.RJ = actionMask.RJ;
            in.actionInput.L2 = actionMask.L2;
            in.actionInput.LJ = actionMask.LJ;

        }else if(e.tipo == EntityType::PLAYERONLINE){
            auto& in = e.getParent().getComponent<InputComponent>(e.getComponentKey<InputComponent>().value());
            auto& mult = e.getParent().getComponent<MultiplayerComponent>(e.getComponentKey<MultiplayerComponent>().value());
            myInput actionMask = mult.inputServer;
            in.actionInput.inputMask = actionMask.inputMask;
            in.actionInput.R2 = actionMask.R2;
            in.actionInput.RJ = actionMask.RJ;
            in.actionInput.L2 = actionMask.L2;
            in.actionInput.LJ = actionMask.LJ;
        }
        
        
        
    }
    
}

void InputSystem::clean_one_entity(E& e){

    if(e.getParent().hasComponent<InputComponent>(e)) {
        auto& in = e.getParent().getComponent<InputComponent>(e.getComponentKey<InputComponent>().value());

        in.actionInput = {0,0,0,0,0};
    }
    if(e.getParent().hasComponent<MultiplayerComponent>(e)) {
        auto& mult = e.getParent().getComponent<MultiplayerComponent>(e.getComponentKey<MultiplayerComponent>().value());

        mult.inputServer = {0,0,0,0};
    }
    
}

void InputSystem::debug_one_entity(E& e){
    auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
    auto& mult = e.getParent().getComponent<MultiplayerComponent>(e.getComponentKey<MultiplayerComponent>().value());
    auto& in = e.getParent().getComponent<InputComponent>(e.getComponentKey<InputComponent>().value());

}

