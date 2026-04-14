#include "SoundComponent.hpp"
#include "../engine/ISound.hpp"


// SoundComponent::SoundComponent()
//     : eventKey(""), isPlaying(false), shouldPlay(false), shouldStop(false) {
//         setParameter("volumeContinuous", 100.0f);
//     }

SoundComponent::SoundComponent(){
    for (auto& sound : sounds )
    {
        sound.eventKey = " ";
        sound.isPlaying = false,
        sound.shouldPlay = false,
        sound.shouldStop = false,
        sound.is3D = false;
    }
}

//Constructor con parámetros
// Pero entonces todos los componentes tienen los mismos componentes ???
// No hay que ponerlo en el constructor sino al crear cada evento, indicar que parámetros tiene supongo
// SoundComponent::SoundComponent(const std::string& name)
//     : eventKey(name), isPlaying(false), shouldPlay(false), shouldStop(false) {
//         setParameter("volumeContinuous", 100.0f);
//     }

SoundComponent::SoundComponent(const int index, const std::string& name)
{
    sounds[index].eventKey = name;
    sounds[index].isPlaying = false,
    sounds[index].shouldPlay = false,
    sounds[index].shouldStop = false,
    sounds[index].is3D = false;
}

// Entonces tendré que inicializarlo después de crearlo por defecto digo yo
// void SoundComponent::inicialize(const std::string name){
//     eventKey = name;
//     isPlaying = false;
//     shouldPlay = false; 
//     shouldStop = false;

//     setParameter("volumeContinuous", 100.0f);
// }

// Manejar parámetros
void SoundComponent::setParameter(const int index, const std::string& name, float value){
    //sounds[index].parameters[name] = value;
    if (index < 0 || index >= MaxSounds) return;
    sounds[index].setParameter(name, value);
}

void SoundComponent::clearParameters(const int index){
    sounds[index].clearParameters();
}

float SoundComponent::getValueParameterByName(const int index, const std::string& name) const{
    //return sounds[index].parameters[name];
    if (index < 0 || index >= MaxSounds) return 0.0f;
    return sounds[index].getParameter(name);
}

void SoundComponent::clearComponent(){

    soundEngine_parent->removeInstanceEvent(*this);
}

void SoundComponent::initParent(ISound* parent){
    soundEngine_parent = parent;
}

ISound* SoundComponent::getParent() {
    return soundEngine_parent;
}

ISound* SoundComponent::getParent() const {
    return soundEngine_parent;
}