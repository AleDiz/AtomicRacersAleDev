#pragma once
#include "../components/SoundComponent.hpp"
#include <string>

struct myVector3;

// Es la fachada del fmodSoundEngine, lo que se utlizá para trabajar con el motor de fmod, solo habrá un puntero en el codigo (main)
class ISound {
public:
    virtual ~ISound() = default;
    virtual void init(const std::string& configFile) =0;
    virtual void update() = 0;
    virtual void shutdown() = 0;

    virtual SoundComponent getSoundComponent(u_int32_t entityID, std::array< const std::string, 5> eventsPaths) = 0;

    virtual void updateSoundParameters(SoundComponent& component) = 0;
    virtual void playEvent(SubSound& subSound) = 0;
    virtual void stopEvent(SubSound& subSound) = 0;

    virtual void loadBank(const std::string& bankPath) = 0;
    virtual void setGroupVolume(const std::string& bankPath, float volume) = 0;

    virtual void stopAllEvents() = 0;

    virtual void removeInstanceEvent(SoundComponent& component) = 0;

    virtual void setListenerAttributes(const myVector3& pos, const myVector3& vel, const myVector3& forward, const myVector3& up) = 0;
    virtual void setEvent3DPosition(SubSound& subSound, const myVector3& pos, const myVector3 forward, const myVector3 up) = 0;
    virtual void setPauseBus(const std::string busPath) = 0;
    virtual void setPlayBus(const std::string busPath) = 0;
};