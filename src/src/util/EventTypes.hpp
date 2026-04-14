#pragma once
enum class EventTypes{
    PLAYSOUND,                  // Se tiene que reproducir un sonido (Más para cinemáticas y eso)
    STOPSOUND,                  // Se tiene que parar un sonido (Más para cinemáticas y eso)
    COLLISIONWITHWALL,          // Colisión contra una pared
    LAUNCHPOWERUP,              // Se ha lanzado un powerUp
    TAKEBOX,                    // Se ha cogido una caja de powerUp
    TRIGGEZONE                  // Se ha pasado por una zona específica del circuito
};

struct EventData {
public:
    int idEntityToAdd;
    EventData(int id) : idEntityToAdd(id){};
    virtual ~EventData() = default;  // Destructor virtual
};

// Estructuras específicas para cada tipo de evento (Intentando tener el menor acoplamiento posible)
struct PlaySoundData : public EventData{
          // ID de la entidad que contiene el componente de sonido que nos interesa
          const int indexSubSound;
    PlaySoundData(int id, int index) : EventData(id), indexSubSound(index){};
};
struct StopSoundData : public EventData{
          // ID de la entidad que contiene el componente de sonido que nos interesa
          const int indexSubSound;
    StopSoundData(int id, int index) : EventData(id), indexSubSound(index){};
};
struct LaunchPowerUpData : public EventData{
    int idPowerUp;      // ID de la entidad que contiene el componente de sonido que nos interesa
    LaunchPowerUpData(int idEntity, int id) : EventData(idEntity), idPowerUp(id){};
};
struct TakeBoxData : public EventData{
    int idBox;          // ID de la entidad que contiene el componente de sonido que nos interesa
    TakeBoxData(int idEntity, int id) : EventData(idEntity), idBox(id){};
};
struct CollisionWithWallData : public EventData{ 
    int idEntity1, idEntity2; 
    CollisionWithWallData(int idEntity, int id1, int id2) : EventData(idEntity), idEntity1(id1), idEntity2(id2){}; 
};
struct TriggerZoneEventData : public EventData{ 
    int idEntityZone; 
    TriggerZoneEventData(int idEntity, int id) : EventData(idEntity), idEntityZone(id){};
};