#pragma once
#include "ISound.hpp"
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include <memory>
#include <iostream>

#include <unordered_map>
#include <array>

#include "../util/BasicUtils.hpp"

class FmodSoundEngine : public ISound {

public:
    FMOD::Studio::System* studioSystem;
    FMOD::System* lowLevelSystem;
    std::unordered_map<std::string, FMOD::Studio::EventDescription*> eventDescriptions;
    std::unordered_map<std::string, FMOD::Studio::EventInstance*> eventInstances;
    
    std::unordered_map<std::string, FMOD::Studio::Bank*> loadedBanks;                       // Guardar los bancos


    void init([[maybe_unused]] const std::string& configFile) override {
        // Crear el sistema de FMOD Studio
        FMOD::Studio::System::create(&studioSystem);
        if(!studioSystem){
            std::cerr << "Error: FMOD Studio system creation failed!" << std::endl;
            return;
        }

        // Inicializar el sistema de FMOD Studio
        FMOD_RESULT resultInit = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
        if (resultInit != FMOD_OK) {
            std::cerr << "Error initializing FMOD Studio system: " << FMOD_ErrorString(resultInit) << std::endl;
            return;
        }

        // Obtener el sistema de bajo nivel
        studioSystem->getCoreSystem(&lowLevelSystem);

        // Inicializar el sistema de bajo nivel
        lowLevelSystem->init(512, FMOD_INIT_NORMAL, nullptr);
        lowLevelSystem->set3DSettings(1.0f, 1.0f, 1.0f);


        // Cargar bancos
        loadBank("bancos/Build/Desktop/Master.strings.bank");
        loadBank("bancos/Build/Desktop/Master.bank");

    }

    // Cargar un banco y guardarlo en un mapa para su acceso posterior si fuera necesario
    void loadBank(const std::string& bankPath) override {
        FMOD::Studio::Bank* bank = nullptr;
        FMOD_RESULT result =studioSystem->loadBankFile(bankPath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);

        if(result != FMOD_OK){
            throw std::runtime_error("Failed to load bank: " + bankPath + ".Error: " + FMOD_ErrorString(result));
        }else{
            loadedBanks[bankPath] = bank;
        }
    }

    void setGroupVolume(const std::string& vcaPath, float volume) override {
        FMOD::Studio::VCA *vca = nullptr;
        FMOD_RESULT result = studioSystem->getVCA(vcaPath.c_str(), &vca);

        if (result != FMOD_RESULT::FMOD_OK || vca == nullptr) 
        {
            std::cerr << "Error: VCA not found for path: " << vcaPath << std::endl;
            return;
        } else {
            vca->setVolume(volume);
        }

        
    }

    // Esto crea un component que es el que manejo en el juego
    // SoundComponent getSoundComponent(u_int32_t entityID, const std::string& eventPath) override {
    //     std::string key = eventPath + std::to_string(entityID);

    //     if(eventInstances.find(key) != eventInstances.end()){

    //         // Crear y devolver el componente pero sin añadir un nuevo evento ni nada
    //         return SoundComponent(0, key);
    //     }

    //     // Si la instancia no existe en el mapa de descripciones
    //     if(eventDescriptions.find(eventPath) == eventDescriptions.end()) {
    //         // Se obtiene la nueva descripción
    //         FMOD::Studio::EventDescription* desc;
    //         studioSystem->getEvent(eventPath.c_str(), &desc);
    //         if (!desc) {
    //             throw std::runtime_error("Error: Event description not found for path: " + eventPath);
    //         }
    //         // Se añade al mapa de descripciones en la posición del path
    //         eventDescriptions[eventPath] = desc;
    //     }

    //     // Se crea la nueva instancia del envento
    //     FMOD::Studio::EventInstance* instance;
    //     eventDescriptions[eventPath]->createInstance(&instance);
    //     if (!instance) {
    //         throw std::runtime_error("Error: Event instance creation failed for path: " + eventPath);
    //     }
    //     // Siepre esta creando instancias no comprueba si ya existe
    //     eventInstances[key] = instance;                             // Se almacena el puntero a la instancia

    //     // Crear y devolver el componente
    //     return SoundComponent(0, key);                                           // Se copia o se mueve al que lo llama

    // }

    SoundComponent getSoundComponent(u_int32_t entityID, std::array< const std::string, 5> eventsPaths) override {
        SoundComponent soundComponent;
        int index = 0;

        for ( const auto& eventPath : eventsPaths )
        {
            if(! eventPath.empty()) {
                std::string key = eventPath + std::to_string(entityID);

                if(eventDescriptions.find(eventPath) == eventDescriptions.end())
                {
                    FMOD::Studio::EventDescription* desc;
                    studioSystem->getEvent(eventPath.c_str(), &desc);
                    if (!desc) {
                        throw std::runtime_error("Error: Event description not found for path: " + eventPath);
                        continue;
                    }
                    eventDescriptions[eventPath] = desc;

                }

                if(eventInstances.find(key) == eventInstances.end())
                {
                    FMOD::Studio::EventInstance* instance;
                    eventDescriptions[eventPath]->createInstance(&instance);
                    if(!instance){
                        std::cerr << "Error: Event instance creation failed for path: " << eventPath << std::endl;
                        continue;
                    }
                    eventInstances[key] = instance;
                }

                soundComponent.addSound(index, key);
            }

                index++;

        }

        return soundComponent;



        // Si la instancia no existe en el mapa de descripciones

        // Se crea la nueva instancia del envento                                           // Se copia o se mueve al que lo llama

    }

    // Se le pasa un componente y los parametros de este componente se le ponene a la instancia almacenada en el motor con la key del component
    // void updateSoundParameters(SoundComponent& component) override {
    //     auto inst = eventInstances.find(component.eventKey);
    //     if( inst == eventInstances.end()) return;
    //     //const std::string& key = component.eventKey;
    //     //auto& instance = eventInstances[key];


    //     auto instance = inst->second;
    //     for(const auto& [paramName, value] : component.parameters){
    //         //component->eventInstance->setParameterByName(paramName.c_str(), value);
    //         instance->setParameterByName(paramName.c_str(), value);
    //     }
    // }

    void updateSoundParameters(SoundComponent& component) override {

        for (auto& s : component.sounds)
        {
            if(!s.eventKey.empty()) 
            {
                auto inst = eventInstances.find(s.eventKey);
                if(inst == eventInstances.end()) return;

                auto instance = inst->second;
                for(const auto& [paramName, value] : s.parameters){
                    instance->setParameterByName(paramName.c_str(), value);
                }
            }

        }
    }

    // void playEvent(SoundComponent& component) override {
    //     auto inst = eventInstances.find(component.eventKey);
    //     if (inst == eventInstances.end()) {
    //         return;
    //     } 
        
    //     auto instance = inst->second;

    //     // Comprobar el estado del evento, para ver si al darle al play ya ha terminado y puedes reproducirlo otra vez
    //     // El isPlaying nunca se pone a false cuando termina la canción 
    //     FMOD_STUDIO_PLAYBACK_STATE state;
    //     instance->getPlaybackState(&state);

    //     // Solo si el evento ha llegado a su fin o ha sido parado manualmente puede volver a reproducirse
    //     if(state == FMOD_STUDIO_PLAYBACK_STOPPED || state == FMOD_STUDIO_PLAYBACK_STOPPING){
    //         instance->start();
    //         component.isPlaying = true;
    //     } else {
            
    //     }

    // }

    void playEvent(SubSound& subSound) override {

        auto inst = eventInstances.find(subSound.eventKey);
        if (inst == eventInstances.end()) {
            return;
        } 

        FMOD_3D_ATTRIBUTES attributes;
        FMOD_RESULT result = inst->second->get3DAttributes(&attributes);
        
        auto instance = inst->second;
        FMOD_STUDIO_PLAYBACK_STATE state;
        instance->getPlaybackState(&state);

        if(state == FMOD_STUDIO_PLAYBACK_STOPPED || state == FMOD_STUDIO_PLAYBACK_STOPPING){
            instance->start();
            subSound.isPlaying = true;
        }
        
    }

    

    // void stopEvent(SoundComponent& component) override {
    //     auto inst = eventInstances.find(component.eventKey);
    //     if( inst == eventInstances.end()) return;
    //     //const std::string& key = component.eventKey;
    //     //auto instance = eventInstances[key];

    //     auto instance = inst->second;
        
    //     if(component.isPlaying) {
    //         //instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    //         instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    //         component.isPlaying = false;
    //     }
    // }

    void stopEvent(SubSound& subSound) override {
        auto inst = eventInstances.find(subSound.eventKey);
        if( inst == eventInstances.end()) return;
        //const std::string& key = component.eventKey;
        //auto instance = eventInstances[key];

        auto instance = inst->second;
        
        if(subSound.isPlaying) {
            //instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
            instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
            subSound.isPlaying = false;
        }
            
        
    }
    

    void update() override {
        studioSystem->update();
        //lowLevelSystem->update();
        FMOD_VECTOR testF;
        lowLevelSystem->get3DListenerAttributes(0, nullptr, nullptr, &testF, nullptr);
    }

    void shutdown() override {
        for(auto& [path, bank] : loadedBanks) {
            bank->unload();
        }
        loadedBanks.clear();
        
        for(auto& [key, instance] : eventInstances){
            instance->release();
        }
        eventInstances.clear();

        eventDescriptions.clear();

        // Liberar el sistema de FMOD Studio
        if(studioSystem){
            studioSystem->release();
            studioSystem = nullptr;
        }

        // Liberar el sistema de bajo nivel
        if(lowLevelSystem){
            lowLevelSystem->release();
            lowLevelSystem = nullptr;
        }
    }

    void stopAllEvents() override {
        for(auto& [key, instance] : eventInstances) {
            //instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
            instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);

        }

        //eventInstances.clear(); // Limpiar la lista despues de detener los eventos, no creo que hay que hacerlo por que si no luego no se pueden volver a dar play
    }

    // void removeInstanceEvent(SoundComponent& component) override {
    //     auto inst = eventInstances.find(component.eventKey);
        
    //     if(inst != eventInstances.end()){
    //         inst->second->release();
    //         eventInstances.erase(inst);

    //     }

    // }

    // Cuando se destruya una entidad
    void removeInstanceEvent(SoundComponent& component) override {
        for (auto& s : component.sounds)
        {
            if (!s.eventKey.empty())
            {
                auto inst = eventInstances.find(s.eventKey);
                if (inst != eventInstances.end())
                {
                    // Primero detener el sonido, por seguridad
                    if(!component.isButton)
                        inst->second->stop(FMOD_STUDIO_STOP_IMMEDIATE);
                    inst->second->release();
                    eventInstances.erase(inst);
                }
                
            }
        }
    }

    
    void setListenerAttributes(const myVector3& pos, const myVector3& vel, const myVector3& forward, const myVector3& up) override {
    
        //const FMOD_VECTOR position = {pos.x, pos.y, pos.z};
        //const FMOD_VECTOR velocity = {0, 0, 0};
        //const FMOD_VECTOR forwardFMOD = {forward.x, forward.y, forward.z};
        //const FMOD_VECTOR upFMOD = {up.x, up.y, up.z};

        //lowLevelSystem->set3DListenerAttributes(0, &position, &velocity, &forwardFMOD, &upFMOD);

        myVector3 forward1 = {
            forward.x * -1.0f,
            forward.y * -1.0f,
            forward.z * -1.0f,
        };
        FMOD_3D_ATTRIBUTES listenerAttr = {};
        listenerAttr.position = {pos.x, pos.y, pos.z};
        listenerAttr.forward = {forward1.x, forward1.y, forward1.z};
        listenerAttr.up = {up.x, up.y, up.z};
        listenerAttr.velocity = {0, 0, 0};

        FMOD_VECTOR position = {pos.x, pos.y, pos.z};
        FMOD_VECTOR velocity = {0, 0, 0};
        FMOD_VECTOR forwardFMOD = {forward1.x, forward1.y, forward1.z};
        FMOD_VECTOR upFMOD = {up.x, up.y, up.z};
        lowLevelSystem->set3DListenerAttributes(0, &position, &velocity, &forwardFMOD, &upFMOD);
        studioSystem->setListenerAttributes(0, &listenerAttr);
        

        FMOD_VECTOR position2 = {};
        FMOD_VECTOR velocity2 = {};
        FMOD_VECTOR forwardFMOD2 = {};
        FMOD_VECTOR upFMOD2 = {};
    }

    const char* playbackStateName(FMOD_STUDIO_PLAYBACK_STATE state) {
        switch (state) {
            case FMOD_STUDIO_PLAYBACK_PLAYING: return "PLAYING";
            case FMOD_STUDIO_PLAYBACK_SUSTAINING: return "SUSTAINING";
            case FMOD_STUDIO_PLAYBACK_STOPPED: return "STOPPED";
            case FMOD_STUDIO_PLAYBACK_STOPPING: return "STOPPING";
            default: return "UNKNOWN";
        }
    }

    // void setEvent3DPosition(SoundComponent& component, const myVector3& pos) override {
    //     auto inst = eventInstances.find(component.eventKey);
    //     if (inst == eventInstances.end()) return;

    //     FMOD_3D_ATTRIBUTES attributes;
    //     attributes.position = {pos.x, pos.y, pos.z};    // Restar la posición del listener
    //     attributes.velocity = {0, 0, 1};
    //     attributes.forward = {0.0f, 0.0f, 1.0f};
    //     attributes.up = {0.0f, 1.0f, 0.0f};

    //     inst->second->set3DAttributes(&attributes);
        
        
    // }

    void setEvent3DPosition(SubSound& subSound, const myVector3& pos, const myVector3 forward, const myVector3 up) override {
        
        auto inst = eventInstances.find(subSound.eventKey);
        if (inst == eventInstances.end()) return;

    
        FMOD_3D_ATTRIBUTES attr = {};
        attr.position = {pos.x, pos.y, pos.z};
        attr.forward = {0, 0, 1};
        attr.up = {0, 1, 0};
        attr.velocity = {0, 0, 0};

        
        inst->second->set3DAttributes(&attr);

        FMOD_3D_ATTRIBUTES attributes2;
        inst->second->get3DAttributes(&attributes2);
        
    
    }

    void setPauseBus(const std::string busPath) override
    {
        FMOD::Studio::Bus* bus = nullptr;
        FMOD::Studio::Bus* testBus = nullptr;
        studioSystem->getBus(busPath.c_str(), &bus);

        if(bus)
        {
            bus->setPaused(true);
        }
    }

    void setPlayBus(const std::string busPath) override
    {
        FMOD::Studio::Bus* bus = nullptr;
        studioSystem->getBus(busPath.c_str(), &bus);

        if(bus)
        {
            bus->setPaused(false);
        }
    }
};