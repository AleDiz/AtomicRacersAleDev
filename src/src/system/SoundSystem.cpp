#include "SoundSystem.hpp"
#include "../engine/FmodSoundEngine.cpp"         // Fachada de sonido FMOD
#include "../Entity/Entity.hpp"                // Entidades
#include "../man/EntityManager.hpp"
#include "../components/SoundComponent.hpp"
#include "../components/ButtonComponent.hpp"


void SoundSystem::update(EManager& EM, ISound* soundEngine){
    // Actualizar los atributos del listener
    update_listenner_atributes(*EM.getEntityByType(EntityType::CAMERA), soundEngine);

    // Itera sobre todas las entidades que tengan un componente de sonido
    EM.forAllCondition<void(*)(E&, ISound*), SoundComponent>(update_one_entity, soundEngine);
}

void SoundSystem::update_menus(EManager& EM, ISound* soundEngine){
    // Actualizar los atributos de                            tener
    // update_listenner_atributes(*EM.getEntityByType(EntityType::CAMERA), soundEngine);

    // Itera sobre todas las entidades que tengan un componente de sonido
    EM.forAllCondition<void(*)(E&, ISound*), SoundComponent>(update_one_entity, soundEngine);
}

void SoundSystem::update_one_entity(E& e, ISound* soundEngine) {
    auto& soundComponent = e.getParent().getComponent<SoundComponent>(e.getComponentKey<SoundComponent>().value());

    // Procesar los eventos de sonido que tiene cada entidad
    processEventsSound(e, soundComponent);

    // Reproducir o actualizar el estado de un sonido
    for (auto& s : soundComponent.sounds)
    {
        if(s.eventKey.empty()) continue;
        
        if(s.shouldStop) 
        {
            if(s.isPlaying)
            {
                soundEngine->stopEvent(s);
            }
            s.shouldStop = false;
        }

        if(s.shouldPlay)
        {
            soundEngine->playEvent(s);
            s.shouldPlay = false;
        }

    }
    // Actualizar parámetros si es necesario
    if ( e.tipo == EntityType::PLAYER) {
        auto& vc = e.getParent().getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());
        soundComponent.sounds[0].parameters["speed"] = vc.m_vehicle->getCurrentSpeedKmHour();
        soundComponent.sounds[0].parameters["throttle"] = vc.throttle;
        soundComponent.sounds[3].parameters["speed"] = vc.m_vehicle->getCurrentSpeedKmHour();


    }
    soundEngine->updateSoundParameters(soundComponent);

    // Actualizar la posición del sonido --------------------------------------------------
    
    // -------------------------------------------------------------------------------------
    

    // SE ENCARGARÁ DE CONTROLAR LOS PARÁMETROS DINÁMICOS COMO LA POSICIÓN DEL SONIDO, VOLUMEN SEGÚN LA DISTANCIA Y ESO
}

void SoundSystem::update_one_volume(E& e, ISound* soundEngine, int v1, int v2, int v3){
    // auto& c = e.getParent().getComponent<SoundComponent>(e.getComponentKey<SoundComponent>().value());


    // float volume = (float)v3;

    // if(c.type == SoundTypes::MUSIC){

    //     volume = (float)v1 * (static_cast<float>(v3)/100.0f);

    //     c.setParameter("volumeContinuous", volume);
    //     soundEngine->updateSoundParameters(c);
    // }else if(c.type == SoundTypes::SFX){

    //     volume = (float)v2 * (float)(v3/100);

    //     c.setParameter("volumeContinuous", volume);
    //     soundEngine->updateSoundParameters(c);
    // }
}

void SoundSystem::changeVolumeAfterMenu(EManager& EM, ISound* soundEngine, int v1, int v2, int v3){
    EM.forAllCondition<void(*)(E&, ISound*, int, int, int), SoundComponent>(update_one_volume, soundEngine, v1, v2, v3);
}

void SoundSystem::removeAllEventInstances(EManager& EM, ISound* soundEngine){
    EM.forAllCondition<void(*)(E&, ISound*), SoundComponent>(remove_one_instance_event, soundEngine);
}

void SoundSystem::remove_one_instance_event(E& v, ISound* soundEngine){
    soundEngine->removeInstanceEvent(v.getParent().getComponent<SoundComponent>(v.getComponentKey<SoundComponent>().value()));
}

// Coge los eventos de la entidad de reproducir su sonido y modifica el componente de sonido
void SoundSystem::processEventsSound(E& e, SoundComponent& soundComponent)
{
    if(e.getParent().hasComponent<EventComponent>(e))
    {
        auto& ec = e.getParent().getComponent<EventComponent>(e.getComponentKey<EventComponent>().value());
        auto const vectorEventStop = ec.getEventDataType<StopSoundData>(); // Todos se refieren a la propia entidad
        for (const auto* data : vectorEventStop)
        {
            // Coger que sonido en el caso de que hayan varios en el mismo componente
            soundComponent.sounds[data->indexSubSound].shouldStop = true;
        }
        ec.deleteEventsType<StopSoundData>();

        auto const vectorEventSound = ec.getEventDataType<PlaySoundData>(); // Todos se refieren a la propia entidad
        for (const auto* data : vectorEventSound)
        {
            // Coger que sonido en el caso de que hayan varios en el mismo componente
            soundComponent.sounds[data->indexSubSound].shouldPlay = true;
        }
        ec.deleteEventsType<PlaySoundData>();

        
    }
}

void SoundSystem::update_listenner_atributes(E& e, ISound* soundEngine){
    auto& camera = e.getParent().getComponent<CameraComponent>(e.getComponentKey<CameraComponent>().value());
    myVector3 pos = {camera.pos.x , camera.pos.y , camera.pos.z };
    myVector3 vel = {0,0,0};
    myVector3 forward = {   camera.target.x - camera.pos.x,
                            camera.target.y - camera.pos.y,
                            camera.target.z - camera.pos.z };
    forward.normalize();

    myVector3 worldRight = forward.cross({0, 1, 0});  // eje X relativo
    worldRight.normalize();

    myVector3 up = worldRight.cross(forward);  // eje Y real
    up.normalize();

    // myVector3 up = camera.up;
    myVector3 testPos = {20.0f, 0, 10}; // Misma posición que el evento

    soundEngine->setListenerAttributes(pos, vel, forward, up);
    
}