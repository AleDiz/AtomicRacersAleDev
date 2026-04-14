#include "ObjectSystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../util/ObjectRigidType.hpp"

void ObjectSystem::update(EManager& EM, float deltaTime){
    EM.forAllCondition<void(*)(E&, float), ObjectComponent>(update_one_entity, deltaTime);
};

void ObjectSystem::interpolate(EManager& EM, float iF){
    EM.forAllCondition<void(*)(E&, float), ObjectComponent>(interpolate_one_entity, iF);
}

void ObjectSystem::savePhysicsPrevious(EManager& EM){
    EM.forAllCondition<void(*)(E&), ObjectComponent>(savePhysicsPrevious_one_entity);
};

void ObjectSystem::savePhysicsCurrent(EManager& EM){
    EM.forAllCondition<void(*)(E&), ObjectComponent>(savePhysicsCurrent_one_entity);
};

void ObjectSystem::update_one_entity(E& e, float deltaTime){
    auto& o = e.getParent().getComponent<ObjectComponent>(e.getComponentKey<ObjectComponent>().value());

    if(o.rigidBody == nullptr){
        return;
    }

    if(o.getRigidBody()->getUserIndex() == ObjectRigidType::TYPE_BOX_POWER_UP){

        if(o.isCollide == false){
            o.temp += deltaTime;
            if(o.temp >= o.timeRespawn){
                o.isCollide = true;
                o.temp = 0.0f;
            }
        }
        
    }

    if(o.getRigidBody()->getUserIndex() == ObjectRigidType::TYPE_POWER_UP_RANDOM){
        if(o.type == SHELL){          
            btVector3 posAct = o.rigidBody->getWorldTransform().getOrigin();
            posAct.setY(2.5f);
            o.rigidBody->getWorldTransform().setOrigin(posAct);
            o.rigidBody->setWorldTransform(o.rigidBody->getWorldTransform());

            if(o.rigidBody->getUserIndex2() == 1)
            {
                e.state = false;
                o.rigidBody->setUserIndex2(0);
            }
        }
        
    }
}

void ObjectSystem::interpolate_one_entity(E& e, float iF){
    auto& oc = e.getParent().getComponent<ObjectComponent>(e.getComponentKey<ObjectComponent>().value());

    iF = std::clamp(iF, 0.0f, 1.0f);

    oc.interpolatedTransform.setOrigin(
        oc.previousTransform.getOrigin().lerp(
            oc.currentTransform.getOrigin(),
            iF
        )
    );

    oc.interpolatedTransform.setRotation(
        oc.previousTransform.getRotation().slerp(
            oc.currentTransform.getRotation(),
            iF
        )
    );
}

void ObjectSystem::savePhysicsPrevious_one_entity(E& e){
       
    auto& oc = e.getParent().getComponent<ObjectComponent>(e.getComponentKey<ObjectComponent>().value());

    oc.previousTransform = oc.currentTransform;

}

void ObjectSystem::savePhysicsCurrent_one_entity(E& e){

    auto& oc = e.getParent().getComponent<ObjectComponent>(e.getComponentKey<ObjectComponent>().value());

    oc.currentTransform = oc.rigidBody->getWorldTransform();
}