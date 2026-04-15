#pragma once

#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include <array>

#include "../util/BasicUtils.hpp"

class ISound;

struct SubSound {
    std::string eventKey;   // Identificador único del evento (path + entityID)
    bool isPlaying{};
    bool shouldPlay{};
    bool shouldStop{};
    bool is3D{};
    std::unordered_map<std::string, float> parameters;
    std::unordered_map<std::string, float> cachedParameters;

    void setParameter(const std::string& name, float value) {
        parameters[name] = value;
    }

    float getParameter(const std::string& name) const {
        auto it = parameters.find(name);
        return (it != parameters.end()) ? it->second : 0.0f;
    }

    void clearParameters() {
        parameters.clear();
    }
};

class SoundComponent {

    public:
        static constexpr int Capacity{100};
        static constexpr int MaxSounds{5};

        // Array de sonidos
        std::array<SubSound, MaxSounds> sounds;
        bool isButton{false};

        // Si es una entidad Sound3D
        myVector3 positionDebug{0, 0, 0};
    
        //FMOD::Studio::EventInstance* eventInstance;
        // No puede tener esto por que sino sería solo para FMOD, es el motor de Fmod quien se queda las instancias en un mapa y luego utilizaré los ids para localizar las instancias
        // El component unicamente tiene la información del evento, que se pone en el getComponentSOund del motor
        // A partir del component se utiliza para actualizar parámetros pasandole el component y consigo el id
        // El motor se encarga de cambiarlo con el component
        // bool isPlaying{};
        // bool shouldPlay{};
        // bool shouldStop{};
        // bool is3D{};
        // std::unordered_map<std::string, float> parameters;
        // std::unordered_map<std::string, float> cachedParameters;

        ISound* soundEngine_parent{nullptr};
        ISound* getParent() const;
        ISound* getParent();

        // Constructor por defecto
        SoundComponent();
        
        //Constructor con parámetros
        // Pero entonces todos los componentes tienen los mismos componentes ???
        // No hay que ponerlo en el constructor sino al crear cada evento, indicar que parámetros tiene supongo
        SoundComponent(const int index, const std::string& name);
        // Entonces tendré que inicializarlo después de crearlo por defecto digo yo
        void inicialize(const std::string name);

        // Manejar parámetros
        void setParameter(const int index, const std::string& name, float value);

        void clearParameters(const int index);

        float getValueParameterByName(const int index, const std::string& name) const;
        
        void clearComponent();

        void initParent(ISound* parent);

        bool addSound(int index, const std::string& key) {
        if (index < 0 || index >= MaxSounds) return false;

            sounds[index].eventKey = key;
            sounds[index].isPlaying = false;
            sounds[index].shouldPlay = false;
            sounds[index].shouldStop = false;
            sounds[index].is3D = false;
            sounds[index].parameters["volumeContinuous"] = 100.0f;
            return true;
        }

};


#endif