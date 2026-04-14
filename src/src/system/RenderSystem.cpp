#include "RenderSystem.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../man/RenderManager.hpp"

void RenderSystem::update(EManager& EM, RenderManager* RM){
    //Falta recorrer las luces y la camara
    EM.forAllCondition<void(*)(E&, RenderManager*),Render3dComponent>   (update_render_one_entity_mesh,  RM);
    EM.forAllCondition<void(*)(E&, RenderManager*),RenderShapeComponent>(update_render_one_entity_shape, RM);
    EM.forAllCondition<void(*)(E&, RenderManager*),ParticlesComponent>  (update_render_one_entity_particle, RM);
    EM.forAllCondition<void(*)(E&, RenderManager*),CameraComponent>     (update_render_one_entity_camera, RM);
    //EM.forAllCondition<void(*)(E&, RenderManager*),RenderLightComponent>(update_render_one_entity_shape,RM);

    EM.forAllCondition<void(*)(E&, RenderManager*), Render2dComponent>  (update_render_one_entity_image, RM);
    EM.forAllCondition<void(*)(E&, RenderManager*), RenderTextComponent>  (update_render_one_entity_text, RM);

    RM->render();
};

void RenderSystem::update_render_one_entity_mesh(E& e, RenderManager* RM){
    auto& rc = e.getParent().getComponent<Render3dComponent>(e.getComponentKey<Render3dComponent>().value());
    btVector3 pos{0, 0, 0};
    btQuaternion rot = btQuaternion::getIdentity();

    if(e.getParent().hasComponent<VehicleComponent>(e)){
        auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
        pos = vc.interpolatedTransform.getOrigin();
        rot = vc.interpolatedTransform.getRotation();

        //Girar el chasis hacia donde este derrapando
        //if(vc.stateVehicle == StateVehicle::DRIFTING){
        if(vc.drifting){
            float driftEffect = vc.steeringDrift * 0.4f;  // Factor de inclinación en Z
            float yawEffect = vc.steeringDrift * 1.2f;    // Factor de giro en Y exagerado
            
            btQuaternion currentRot = rot;
        
            // Crear las rotaciones objetivo
            btQuaternion driftTilt(btVector3(0, 0, 1), driftEffect);
            btQuaternion driftYaw(btVector3(0, 1, 0), yawEffect); 
        
            // Aplicar la inclinación y el giro
            btQuaternion targetRot = driftYaw * currentRot * driftTilt;
        
            rot = targetRot;
        }
        

    }else if(e.getParent().hasComponent<ObjectComponent>(e)){
        auto& oc = e.getParent().getComponent<ObjectComponent>(e.getComponentKey<ObjectComponent>().value());
        
        if(e.tipo == EntityType::OBJETOCOLISIONABLE){
            pos = oc.interpolatedTransform.getOrigin();
            rot = oc.rotationInit ;
        }else{
            pos = btVector3(oc.posInc.x, oc.posInc.z, oc.posInc.y);
            rot = oc.rigidBody->getWorldTransform().getRotation();
        }
        
        if(!oc.isCollide){
            return;
        }
    }else if(e.getParent().hasComponent<NotAIComponent>(e)){
        auto& nAI = e.getParent().getComponent<NotAIComponent>(e.getComponentKey<NotAIComponent>().value());
        
        pos.setX(nAI.posActual.x);
        pos.setY(nAI.posActual.y);
        pos.setZ(nAI.posActual.z);

        rot.setX(nAI.rot.axis.x);
        rot.setY(nAI.rot.axis.y);
        rot.setZ(nAI.rot.axis.z);
        rot.setW(nAI.rot.angle);
        
    }else if(e.tipo == EntityType::MUESTRA){
        pos = {0.0f, 0.0f, 0.0f};
        float angle = 0.3f * RM->getWindowTime(); // tiempo acumulado
        rot= {btVector3(0, 1, 0), angle};
    }

    myMesh thisMesh;
    thisMesh.id = rc.id;
    thisMesh.position = myVector3{pos.getX(),pos.getY(),pos.getZ()};
    thisMesh.rotation = myQuaternion{rot.getAngle(),myVector3{rot.getAxis().getX(),rot.getAxis().getY(),rot.getAxis().getZ()}};

    //Añadir al vector de renderManager
    RM->addRenderData(thisMesh);
};

void RenderSystem::update_render_one_entity_shape(E& e, RenderManager* RM){
    auto& rc = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
    btVector3 pos{0, 0, 0};
    btQuaternion rot = btQuaternion::getIdentity();


    if(e.getParent().hasComponent<VehicleComponent>(e)){
        auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
        pos = vc.interpolatedTransform.getOrigin();
        rot = vc.interpolatedTransform.getRotation();

        //Girar el chasis hacia donde este derrapando
        if(e.getParent().hasComponent<AIComponent>(e)){
            auto& renderShape = e.getParent().getComponent<RenderShapeComponent>(e.getComponentKey<RenderShapeComponent>().value());
            RM->addRenderData(renderShape.shape);
        }

    }else if(e.getParent().hasComponent<ObjectComponent>(e)){
        auto& oc = e.getParent().getComponent<ObjectComponent>(e.getComponentKey<ObjectComponent>().value());
        pos = btVector3(oc.posInc.x, oc.posInc.z, oc.posInc.y);
        rot = oc.rigidBody->getWorldTransform().getRotation();
        if(!oc.isCollide){
            return;
        }
    }else if(e.getParent().hasComponent<WaypointComponent>(e)){
        auto& wc = e.getParent().getComponent<WaypointComponent>(e.getComponentKey<WaypointComponent>().value());
        pos = wc.rigidBody->getWorldTransform().getOrigin();
        rot = wc.rigidBody->getWorldTransform().getRotation();
    }
    

    myShape thisShape = rc.shape;
    thisShape.position = myVector3{pos.getX(),pos.getY(),pos.getZ()};
    thisShape.rotation = myQuaternion{rot.getAngle(),myVector3{rot.getAxis().getX(),rot.getAxis().getY(),rot.getAxis().getZ()}};

    if(e.tipo == EntityType::SOUND3DSTATIC)
    {
        auto& sc = e.getParent().getComponent<SoundComponent>(e.getComponentKey<SoundComponent>().value());
        thisShape.position = myVector3{sc.positionDebug.x, sc.positionDebug.y, sc.positionDebug.z};
    }

    //Añadir al vector de renderManager
    RM->addRenderData(thisShape);
};

void RenderSystem::update_render_one_entity_camera(E& e,RenderManager* RM){
    auto& cc = e.getParent().getComponent<CameraComponent>(e.getComponentKey<CameraComponent>().value());
    
    myCamera camera {};
    camera.fovy       = cc.fovy;
    camera.position   = cc.pos;
    camera.projection = cc.projection;
    camera.target     = cc.target;
    camera.up         = cc.up; 

    RM->addRenderData(camera);
};

void RenderSystem::update_render_one_entity_light(E& e,RenderManager* RM){
    //WARNING ALE - cuando se añada la logica de las luces acabar esto
};

void RenderSystem::update_render_one_entity_image(E& e, RenderManager* RM){
    auto& rc = e.getParent().getComponent<Render2dComponent>(e.getComponentKey<Render2dComponent>().value());

    bool hover {false};

    if(e.getParent().hasComponent<ButtonComponent>(e)){
        auto& bc = e.getParent().getComponent<ButtonComponent>(e.getComponentKey<ButtonComponent>().value());
        hover = bc.hover;
    }

    myImage image {};
    image.id = rc.id;
    image.hover = hover;
    image.position = rc.position;
    image.color = rc.color;

    RM->addRenderData(image);
}

void RenderSystem::update_render_one_entity_text(E& e, RenderManager* RM){
    auto& rc = e.getParent().getComponent<RenderTextComponent>(e.getComponentKey<RenderTextComponent>().value());
    RM->addRenderData(rc.text);
}


void RenderSystem::update_render_one_entity_particle(E& e, RenderManager* RM){
    auto& rc = e.getParent().getComponent<ParticlesComponent>(e.getComponentKey<ParticlesComponent>().value());
    btVector3 pos{0, 0, 0};
    btQuaternion rot = btQuaternion::getIdentity();

    for( auto& pt : rc.particles_ ){        
        pos.setX(pt.position_.x);
        pos.setY(pt.position_.y);
        pos.setZ(pt.position_.z);

        myParticle thisParticle;
        thisParticle.id = rc.idModel;
        thisParticle.position = myVector3{pos.getX(),pos.getY(),pos.getZ()};
        thisParticle.rotation = myQuaternion{rot.getAngle(),myVector3{rot.getAxis().getX(),rot.getAxis().getY(),rot.getAxis().getZ()}};

        //Añadir al vector de renderManager
        RM->addRenderData(thisParticle);
    }
}
