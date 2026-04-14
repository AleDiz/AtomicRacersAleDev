#include "EntityFactory.hpp"
#include "../man/EntityManager.hpp"
#include "../Entity/Entity.hpp"
#include "random"

void EntityFactory::creationRequestprocess(EManager& EM,  btDynamicsWorld* dynamicsWorld, ISound* soundEngine){
    
    for (auto const& e : EM.getCreationQueue()) {

        if(EM.hasComponent<VehicleComponent>(e)){
            vehicleCreations(e, EM, dynamicsWorld, soundEngine);
        }
    }


    for (auto const& e : EM.getParticlesCreationQueue()) {
        auto const id = std::get<0>(e);
        auto const pType  = std::get<1>(e);

        particleCreations(EM, pType, id);
    }

    EM.clearCreationQueue();
    EM.clearParticlesCreationQueue();
}

void EntityFactory::creationOnlineRequestprocess(EManager& EM, NetEngine& NE, btDynamicsWorld* dynamicsWorld, ISound* soundEngine){
    
    for (auto const& e : EM.getOnlineCreationQueue()) {

        if(e.nUniqueID == NE.getnPlayerID())
        {
            CreateOwnOnlineVehicle(EM, dynamicsWorld, soundEngine, e.position, e.playerPos, e.nModelCarID, e.playerName, e.nUniqueID);
        }else{
            CreateOnlineVehicle(EM, dynamicsWorld, soundEngine, e.position, e.playerPos, e.nModelCarID, e.playerName, e.nUniqueID);
        }
    }
    EM.clearOnlineCreationQueue();
}

void EntityFactory::vehicleCreations(const E& e, EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine){
    
    auto& v = EM.getComponent<VehicleComponent>(e.getComponentKey<VehicleComponent>().value());

    switch (v.powerUp)
    {
        case PowerUps::ANYONE:
        case PowerUps::BOOSTX3:
        case PowerUps::BANANAX3:
        case PowerUps::SHELLX3:
            break;
        case PowerUps::BANANA:
        {
            // Obtenemos la posición actual del coche
            btVector3 position = v.m_vehicle->getChassisWorldTransform().getOrigin();
            btQuaternion rotation = v.m_vehicle->getChassisWorldTransform().getRotation();
            float rot = rotation.getZ();

            // Obtenemos la dirección hacia donde apunta el coche
            btVector3 direction = v.m_vehicle->getForwardVector();

            // Añadimos un offset hacia adelante usando el vector de dirección
            float forwardOffset = -8.0f;                        // Distancia deseada hacia adelante
            position += direction.normalized() * forwardOffset; // Offset dinámico basado en la dirección

            // Añadimos un offset vertical opcional (si necesario, por encima del coche)
            position += btVector3(0.0f, 1.0f, 0.0f);

            myVector3 pos {
                position.getX(),
                position.getZ(),
                position.getY()
            };


            
            auto &o = createPowerUpObject(EM, dynamicsWorld, pos, "box", 200, soundEngine, ObjectRigidType::TYPE_POWER_UP_RANDOM, PowerUps::BANANA, 8.0f);

            EM.getComponent<ObjectComponent>(o.getComponentKey<ObjectComponent>().value()).rotationInit = rotation;

            //soundEngine->playEvent(EM.getComponent<SoundComponent>(o.getComponentKey<SoundComponent>().value()));

            break;
        }
        case PowerUps::BOOST:
        {
            v.powerUpUsed = PowerUps::BOOST;

            break;
        }

        case PowerUps::COHETE:
        {
            // Hacer que sea por un tiempo y no tengas que estar manteniendo la F
            v.powerUpUsed = PowerUps::COHETE;
            
            auto *c = EM.getEntityByType(EntityType::PLAYER);

            // Eliminar el componente de input
            EM.delete_one_component<InputComponent>(*c);

            // Añadir el componente de IA
            auto aiKey = EM.addComponent<AIComponent>(*c);  // Guardamos la clave en la entidad

            if (auto aiComponent = c->getComponentKey<AIComponent>()) {  
                auto& ai = EM.getComponent<AIComponent>(aiComponent.value());

                // Configurar el AIComponent con PathFollowing
                //ai.actions = createActions();
                ai.waypoint2Go = EM.getComponent<VehicleComponent>(c->getComponentKey<VehicleComponent>().value()).waypointSiguiente;
                ai.arrivalRadius = 1;
            }

            // Configuración de velocidad y tiempos
            v.timerPowerUp = std::chrono::high_resolution_clock::now();
            v.maxSpeed = MAXSPEEDPLAYERBOOST;

            break;
        }

        case PowerUps::SHELL:
        {
            // Obtenemos la posición actual del coche
            btVector3 position = v.m_vehicle->getChassisWorldTransform().getOrigin();
            btQuaternion rotation = v.m_vehicle->getChassisWorldTransform().getRotation();
            float rot = rotation.getZ();

            // Obtenemos la dirección hacia donde apunta el coche
            btVector3 direction = v.m_vehicle->getForwardVector();

            // Añadimos un offset hacia adelante usando el vector de dirección
            float forwardOffset = 8.0f;                         // Distancia deseada hacia adelante
            position += direction.normalized() * forwardOffset; // Offset dinámico basado en la dirección

            // Añadimos un offset vertical opcional (si necesario, por encima del coche)
            position += btVector3(0.0f, 1.0f, 0.0f);

            // Creamos el objeto power-up con el factory
           
            myVector3 pos {
                position.getX(),
                position.getZ(),
                position.getY()
            };


            // MIRAR A VER SI PODEMOS PONER LOS DATOS A RELLENAR DINAMICAMENTE Y NO A PELO
            auto &o = createPowerUpObject(EM, dynamicsWorld, pos, "box", 201, soundEngine, ObjectRigidType::TYPE_POWER_UP_RANDOM, PowerUps::SHELL, 8.0f);
            EM.getComponent<ObjectComponent>(o.getComponentKey<ObjectComponent>().value()).rotationInit = rotation;
            


            //soundEngine->playEvent(EM.getComponent<SoundComponent>(o.getComponentKey<SoundComponent>().value()));

            // Calculamos la velocidad del objeto
            float speed = 75.0f; // Velocidad deseada
            btVector3 velocity = direction.normalized() * speed;

            // Establecemos la velocidad lineal del objeto
            EM.getComponent<ObjectComponent>(o.getComponentKey<ObjectComponent>().value()).rigidBody->setLinearVelocity(velocity);


            break;
        }
    }

    if (v.tamPowerUp - 1 == 0)
    {
        v.powerUp = static_cast<PowerUps>(0);
        v.powerUpRender = static_cast<PowerUps>(0);
    }
    else
    {
        v.tamPowerUp -= 1;
    }

}

//CREACION DE COCHES DEL OFFLINE ---------------------------------------

E& EntityFactory::CreateVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine, 
    myVector3 posicion, int pos, int id, const char* nombre)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::PLAYER;

    EM.addEntityToAux1();

    // Configuración de componentes
    EM.addComponent<VehicleComponent>(entity);
    EM.addComponent<InputComponent>(entity);
    EM.addComponent<SoundComponent>(entity);
    EM.addComponent<EventComponent>(entity);
    
    
    // EM.addComponent<RenderShapeComponent>(entity);
    // EM.addComponent<AIComponent>(entity);
    // auto& ia = EM.getComponent<AIComponent>(entity.getComponentKey<AIComponent>().value());
    // ia.initAIComponent(5.0f, 0.5f, 20.0f, 0.3f);

    // auto& renderShape = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
    // renderShape.initRender3dComponent(1.0f, 1.0f, 1.0f, {0, 0 , 255, 255});
    
    auto& vehicle = EM.getComponent<VehicleComponent>(entity.getComponentKey<VehicleComponent>().value());

    btVector3 positionInitial {
        posicion.x,
        posicion.y,
        posicion.z
    };
    //WARNING ALE
    std::string typeVehicle {};
    if(id != 0) {
        EM.addComponent<Render3dComponent>(entity);
        if(id < 510){            //Coches pequeños 500 - 510
            typeVehicle = "small";
        }else if(id >= 520){     //Coches grande 520 - 530
            typeVehicle = "large";
        }else{                  //Coches medianos 510 - 520
            typeVehicle = "medium";
        }
        auto& render = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
        render.initRender3dComponent(id);
    }else{
        EM.addComponent<RenderShapeComponent>(entity);
        auto& render = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
        render.initRender3dComponent(1.0f, 1.0f, 4.0f, myColor{0, 255, 0, 255});
    }
    
    // ------------- CARGAR VALORES FÍSICA TIPO COCHE -------------
    JSONManager jsonConfigurationVehicle;
    jsonConfigurationVehicle.loadJSON("assets/JSON/configurationVehicle.json");
    std::unordered_map<std::string, float> configurationVehicle = jsonConfigurationVehicle.getVehicleConfiguration(typeVehicle);
    // ------------------------------------------------------------

    vehicle.setVehicleConfigurationValues(configurationVehicle);
    vehicle.initPhysics(dynamicsWorld, positionInitial);
    vehicle.posicion = pos;
    vehicle.nombre   = nombre;

    std::string eventPath = "event:/Efectos/AI_Prueba";                                           //Ruta del evento
    std::string eventPath1 = "event:/Efectos/derrape";                                           //Ruta del evento
    std::string eventPath2 = "event:/Efectos/boost-drift";                                           //Ruta del evento
    std::string eventPath3 = "event:/Ambientes/Viento";                                           //Ruta del evento
    std::string eventPath4 = "event:/Efectos/choque";                                           //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, eventPath1, eventPath2, eventPath3, eventPath4};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent); 
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    sound.sounds[0].shouldPlay = true;
    sound.sounds[3].shouldPlay = true;
    //sound.sounds[1].shouldPlay = true;

    if (vehicle.m_carChassis != nullptr) {
        vehicle.m_carChassis->setUserPointer(&entity);
    }

    vehicle.m_carChassis->setUserIndex3(entity.id);

    return entity;
}


E& EntityFactory::CreateIAVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, int posicion, const char* nombre,
    ISound* soundEngine, float arrivalRadius, float time2arrive, float visionDistance, float time2Update, myVector3 spawn, int id)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::IA;

    EM.addEntityToAux1();


    // Configuración de componentes
    EM.addComponent<VehicleComponent>(entity);
    EM.addComponent<AIComponent>(entity);
    EM.addComponent<EventComponent>(entity);
    EM.addComponent<SoundComponent>(entity);

    auto& vehicle = EM.getComponent<VehicleComponent>(entity.getComponentKey<VehicleComponent>().value());
    auto& ia = EM.getComponent<AIComponent>(entity.getComponentKey<AIComponent>().value());

    //auto acciones1 = createActions();
    vehicle.powerUp = PowerUps::ANYONE;
    //ia.actions = acciones1;

    ia.initAIComponent(arrivalRadius, time2arrive, visionDistance, time2Update);

    //Vector3 p = config.getVector3(std::to_string(posicion));

    btVector3 positionInitial {
        spawn.x,
        spawn.y,
        spawn.z
    };

    //WARNING ALE
    std::string typeVehicle {};
    if(id != 0) {
        EM.addComponent<Render3dComponent>(entity);
        if(id < 510){            //Coches pequeños 500 - 510
            typeVehicle = "small";
        }else if(id >= 520){     //Coches grande 520 - 530
            typeVehicle = "large";
        }else{                  //Coches medianos 510 - 520
            typeVehicle = "medium";
        }
        auto& render = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
        render.initRender3dComponent(id);
    }else{
        EM.addComponent<RenderShapeComponent>(entity);
        auto& render = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
        render.initRender3dComponent(1.0f, 1.0f, 4.0f, myColor{0, 255, 0, 255});
    }
    
    // ------------- CARGAR VALORES FÍSICA TIPO COCHE -------------
    JSONManager jsonConfigurationVehicle;
    jsonConfigurationVehicle.loadJSON("assets/JSON/configurationVehicle.json");
    std::unordered_map<std::string, float> configurationVehicle = jsonConfigurationVehicle.getVehicleConfiguration(typeVehicle);
    // ------------------------------------------------------------

    vehicle.setVehicleConfigurationValues(configurationVehicle);
    vehicle.initPhysics(dynamicsWorld, positionInitial);
    vehicle.posicion = posicion;
    vehicle.nombre = nombre;

    vehicle.gozBoost = entity.id + rand()%2 > 0;

    std::string eventPath = "event:/Efectos/AI_prueba";                                           //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {"", "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent); 
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    //sound.sounds[0].shouldPlay = true;
    //sound.sounds[0].is3D = true;

    if (vehicle.m_carChassis != nullptr) {
        vehicle.m_carChassis->setUserPointer(&entity);
    }

    vehicle.m_carChassis->setUserIndex3(entity.id);

    return entity;
}

//CREACION COCHES DEL ONLINE -------------------------------------------------

E& EntityFactory::CreateOnlineVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine, 
    myVector3 posicion, int pos, int idModelo, const char* nombre, uint32_t nUniqueID)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::PLAYERONLINE;

    EM.addEntityToAux1();

    // Configuración de componentes
    EM.addComponent<VehicleComponent>(entity);
    EM.addComponent<InputComponent>(entity);
    EM.addComponent<MultiplayerComponent>(entity);
    EM.addComponent<EventComponent>(entity);
    EM.addComponent<SoundComponent>(entity);

    auto& vehicle = EM.getComponent<VehicleComponent>(entity.getComponentKey<VehicleComponent>().value());
    auto& multiP  = EM.getComponent<MultiplayerComponent>(entity.getComponentKey<MultiplayerComponent>().value());

    btVector3 positionInitial {
        posicion.x,
        posicion.y,
        posicion.z
    };
    //WARNING ALE
    std::string typeVehicle {};
    if(idModelo != 0) {
        EM.addComponent<Render3dComponent>(entity);
        if(idModelo < 510){            //Coches pequeños 500 - 510
            typeVehicle = "small";
        }else if(idModelo >= 520){     //Coches grande 520 - 530
            typeVehicle = "large";
        }else{                  //Coches medianos 510 - 520
            typeVehicle = "medium";
        }
        auto& render = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
        render.initRender3dComponent(idModelo);
    }else{
        EM.addComponent<RenderShapeComponent>(entity);
        auto& render = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
        render.initRender3dComponent(1.0f, 1.0f, 4.0f, myColor{0, 255, 0, 255});
    }
    
    // ------------- CARGAR VALORES FÍSICA TIPO COCHE -------------
    JSONManager jsonConfigurationVehicle;
    jsonConfigurationVehicle.loadJSON("assets/JSON/configurationVehicle.json");
    std::unordered_map<std::string, float> configurationVehicle = jsonConfigurationVehicle.getVehicleConfiguration(typeVehicle);
    // ------------------------------------------------------------

    vehicle.setVehicleConfigurationValues(configurationVehicle);
    vehicle.initPhysics(dynamicsWorld, positionInitial);
    vehicle.posicion = pos;
    vehicle.nombre   = nombre;

    std::string eventPath = "event:/Efectos/AI_Prueba";                                           //Ruta del evento
    std::string eventPath1 = "event:/Efectos/derrape";                                           //Ruta del evento
    std::string eventPath2 = "event:/Efectos/boost-drift";                                           //Ruta del evento
    std::string eventPath3 = "event:/Ambientes/Viento";                                           //Ruta del evento
    std::string eventPath4 = "event:/Efectos/choque";                                           //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, eventPath1, eventPath2, eventPath3, eventPath4};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent); 
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    sound.sounds[0].shouldPlay = true;
    sound.sounds[3].shouldPlay = true;
    //sound.sounds[1].shouldPlay = true;

    if (vehicle.m_carChassis != nullptr) {
        vehicle.m_carChassis->setUserPointer(&entity);
    }

    vehicle.m_carChassis->setUserIndex3(entity.id);

    multiP.initComponent(nUniqueID);

    return entity;
}

E& EntityFactory::CreateOwnOnlineVehicle(EManager& EM, btDynamicsWorld* dynamicsWorld, ISound* soundEngine, 
    myVector3 posicion, int pos, int idModelo, const char* nombre, uint32_t nUniqueID)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::PLAYER;

    EM.addEntityToAux1();

    // Configuración de componentes
    EM.addComponent<VehicleComponent>(entity);
    EM.addComponent<InputComponent>(entity);
    EM.addComponent<MultiplayerComponent>(entity);
    EM.addComponent<SoundComponent>(entity);
    EM.addComponent<EventComponent>(entity);

    auto& vehicle = EM.getComponent<VehicleComponent>(entity.getComponentKey<VehicleComponent>().value());
    auto& multiP  = EM.getComponent<MultiplayerComponent>(entity.getComponentKey<MultiplayerComponent>().value());

    btVector3 positionInitial {
        posicion.x,
        posicion.y,
        posicion.z
    };

    //WARNING ALE
    std::string typeVehicle {};
    if(idModelo != 0) 
    {
        EM.addComponent<Render3dComponent>(entity);
        if(idModelo < 510)
        {            //Coches pequeños 500 - 510
            typeVehicle = "small";
        }else if(idModelo >= 520)
        {     //Coches grande 520 - 530
            typeVehicle = "large";
        }else
        {                  //Coches medianos 510 - 520
            typeVehicle = "medium";
        }
        auto& render = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
        render.initRender3dComponent(idModelo);
    }
    else
    {
        EM.addComponent<RenderShapeComponent>(entity);
        auto& render = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
        render.initRender3dComponent(1.0f, 1.0f, 4.0f, myColor{0, 255, 0, 255});
    }
    
    // ------------- CARGAR VALORES FÍSICA TIPO COCHE -------------
    JSONManager jsonConfigurationVehicle;
    jsonConfigurationVehicle.loadJSON("assets/JSON/configurationVehicle.json");
    std::unordered_map<std::string, float> configurationVehicle = jsonConfigurationVehicle.getVehicleConfiguration(typeVehicle);
    // ------------------------------------------------------------

    vehicle.setVehicleConfigurationValues(configurationVehicle);
    vehicle.initPhysics(dynamicsWorld, positionInitial);
    vehicle.posicion = pos;
    vehicle.nombre = nombre;

    std::string eventPath = "event:/Efectos/derrape";                                           //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent); 
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    //

    if (vehicle.m_carChassis != nullptr) {
        vehicle.m_carChassis->setUserPointer(&entity);
    }

    vehicle.m_carChassis->setUserIndex3(entity.id);


    multiP.initComponent(nUniqueID);

    return entity;
}

// IMPORTANTE
//-----------
// Para crear objetos antes del inicio del juego.
// Posicion: si es objeto del cargador de niveles (0,0,0)
// Shade: ("NoAuto") si no es del cargador de niveles
E& EntityFactory::createObjectNw(EManager& EM,  btDynamicsWorld* dynamicsWorld, myVector3 position, float rotationZ,
        const std::string& shade, int idObj, ISound* soundEngine,
        ObjectRigidType objectRigidType, float mass, bool isbox)
{
    E& entity = EM.CreateEntity();

    if (isbox)
    {
        entity.tipo = EntityType::POWERUP;
    }else
    {
        entity.tipo = EntityType::OBJETOSMUNDO;
        if(objectRigidType == ObjectRigidType::TYPE_GROUND_INFINITY)
        {
            entity.tipo = EntityType::GROUNDINFINITY;
        }
        if(objectRigidType == ObjectRigidType::TYPE_BOOST_GROUND){
            entity.tipo = EntityType::BOOSTGROUND;
        }
        if(shade == "mesh"){
            entity.tipo = EntityType::ROAD;
        }
    }

    EM.addComponent<ObjectComponent>(entity);
    auto& o = EM.getComponent<ObjectComponent>(entity.getComponentKey<ObjectComponent>().value());

    if(objectRigidType == ObjectRigidType::TYPE_GROUND_INFINITY){
        //EM.addComponent<RenderShapeComponent>(entity);
        //auto& r = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
        //r.initRender3dComponent(0, 600, 700, {80,80,80,255});
    }else {
        EM.addComponent<Render3dComponent>(entity);
        auto& r = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
        r.initRender3dComponent(idObj);       // ID de referencia para el ManagerTexturas
    }
    EM.addComponent<EventComponent>(entity);

    o.type = PowerUps::ANYONE;

    if(objectRigidType == ObjectRigidType::TYPE_BOX_POWER_UP || (objectRigidType == ObjectRigidType::TYPE_BOOST_GROUND)){
        std::string eventPath = {};                                           //Ruta del evento
        if(objectRigidType == ObjectRigidType::TYPE_BOX_POWER_UP)
            eventPath = "event:/Efectos/cristal-boxPowerup";
        else if(objectRigidType == ObjectRigidType::TYPE_BOOST_GROUND)
            eventPath = "event:/Efectos/boost-ground";
            
        std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
        auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

       // Añadir el componente al EntityManager
        EM.addComponent<SoundComponent>(entity);
        auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
        sound = std::move(soundComponent);     
                                                // Mover el componente al slot correspondiente
        sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
        sound.sounds[0].is3D = true;
        sound.sounds[0].shouldPlay = false;

        soundEngine->setEvent3DPosition(sound.sounds[0], {position.x, position.z, position.y}, {0, 0, 1}, {0, 1, 0});

        
    }
    // Llamar a initPhysics
    o.initPhysics(dynamicsWorld, position, myVector3(0.0f, 0.0f, 0.0f), entity.id, shade, idObj, rotationZ, objectRigidType, mass);

    o.rigidBody->setUserPointer(&entity);

    return entity;
}

E& EntityFactory::createPowerUpObject(EManager& EM,  btDynamicsWorld* dynamicsWorld, myVector3 position,
    const std::string& shade, int idObj, ISound* soundEngine,
    ObjectRigidType objectRigidType, PowerUps type, float mass) 
{
    // Añadir los tipos de componentes
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::OBJETOCOLISIONABLE;

    EM.addComponent<ObjectComponent>(entity);
    auto& o = EM.getComponent<ObjectComponent>(entity.getComponentKey<ObjectComponent>().value());
    EM.addComponent<Render3dComponent>(entity);
    auto& r = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
    r.initRender3dComponent(idObj);

    // Establecer el tipo e inicializar físicas e id
    o.type = type;
    
    // Obtener el soundComponent
    std::string eventPath {};                                            //Ruta del evento

    switch(type)
    {
        case PowerUps::SHELL:
        {
            eventPath = "event:/Efectos/misil-lanzar";
            break;
        }
        case PowerUps::BANANA:
        {
            eventPath = "event:/Efectos/aceite";
            break;
        }
    }

    std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    EM.addComponent<SoundComponent>(entity);
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent);     
                                             // Mover el componente al slot correspondiente
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    sound.sounds[0].is3D = true;
    sound.sounds[0].shouldPlay = true;

    soundEngine->setEvent3DPosition(sound.sounds[0], {position.x, position.z, position.y}, {0, 0, 1}, {0, 1, 0});

    o.initPhysics(dynamicsWorld, position, myVector3(0.0f, 0.0f, 0.0f), entity.id, "box", 45, 0.0f, objectRigidType, mass);

    return entity;
}

E& EntityFactory::createWaypoint(EManager& EM,  btDynamicsWorld* dynamicsWorld, myVector4 pos, 
    btVector3 size, int id, int idObj, ObjectRigidType objectRigidType, float mass) 
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::WAYPOINT;

    EM.addEntityToAux2();

    EM.addComponent<WaypointComponent>(entity);
    auto& o = EM.getComponent<WaypointComponent>(entity.getComponentKey<WaypointComponent>().value());
    o.id = id;

  
    myVector3 position {pos.x, pos.z, pos.y};
    myVector3 sizeVec3 {size.getX(),size.getY(), size.getZ()};
    float rotationZ = pos.w;
    o.initPhysics(dynamicsWorld, position, sizeVec3, entity.id, idObj, rotationZ, objectRigidType, mass);
    
    /*
    EM.addComponent<RenderShapeComponent>(entity);
    auto &r = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
    r.initRender3dComponent(6.0f, 1.0f, 1.0f, myColor{0, 255, 0, 255});
    */

    return entity;
}

E& EntityFactory::createEntityMusic(EManager& EM,  ISound* soundEngine, const std::string& pathEvent) 
{
    // Añadir los tipos de componentes
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::SOUND;
    EM.addComponent<EventComponent>(entity);
    EM.addComponent<SoundComponent>(entity);
    // Obtener el soundComponent
    std::string eventPath = pathEvent;                                            //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent);
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente

    return entity;
}

E& EntityFactory::createEntitySound3D(EManager& EM,  ISound* soundEngine, const std::string& pathEvent, const myVector4 pos) 
{
    // Añadir los tipos de componentes
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::SOUND3DSTATIC;
    EM.addComponent<EventComponent>(entity);
    EM.addComponent<SoundComponent>(entity);
    // Obtener el soundComponent
    std::string eventPath = pathEvent;                                            //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent);
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    
    sound.sounds[0].shouldPlay = true;
    sound.sounds[0].is3D = true;
    sound.positionDebug = {pos.x, pos.z, pos.y};

    EM.addComponent<RenderShapeComponent>(entity);
    auto& renderShape = EM.getComponent<RenderShapeComponent>(entity.getComponentKey<RenderShapeComponent>().value());
    renderShape.initRender3dComponent(10.0f, 10.0f, 10.0f, {0, 255 , 255, 255});

    soundEngine->setEvent3DPosition(sound.sounds[0], {pos.x, pos.z, pos.y}, {0, 0, 1}, {0, 1, 0});

    return entity;
}


E& EntityFactory::createEntityActivationSound(EManager& EM, btDynamicsWorld* dynamicsWorld,  ISound* soundEngine, const std::string& pathEvent, const myVector4 pos, const int idObj, const myVector4 pos3d) 
{
    // Añadir los tipos de componentes
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::SOUND3DSTATIC;
    EM.addComponent<EventComponent>(entity);
    EM.addComponent<SoundComponent>(entity);
    // Obtener el soundComponent
    std::string eventPath = pathEvent;                                            //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent);
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente

    sound.sounds[0].shouldPlay = false;
    sound.sounds[0].is3D = true;
    sound.positionDebug = {pos.x, pos.z, pos.y};

    EM.addComponent<ObjectComponent>(entity);
    auto& o = entity.getParent().getComponent<ObjectComponent>(entity.getComponentKey<ObjectComponent>().value());
    o.initPhysics(dynamicsWorld, {pos.x, pos.y, pos.z}, myVector3(0.0f, 0.0f, 0.0f), entity.id, "box", 1000, 0.0f, ObjectRigidType::TYPE_SOUND_ACTIVATION, 1.0f);


    soundEngine->setEvent3DPosition(sound.sounds[0], {pos3d.x, pos3d.z, pos3d.y}, {0, 0, 1}, {0, 1, 0});

    return entity;
}


// Entidades que unicamente se renderizan. No como los objetos que se renderizan y tienen colisión
E& EntityFactory::createEntityOnlyRender(EManager& EM, int idRender)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::RENDER;
    EM.addComponent<Render3dComponent>(entity);
    auto& render = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
    render.initRender3dComponent(idRender);
    return entity;
}

E& EntityFactory::createBoostGround (EManager& EM,  btDynamicsWorld* dynamicsWorld, ISound* soundEngine, myVector4 pos, btVector3 size, int idObj,  ObjectRigidType objectRigidType, float mass) 
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::BOOSTGROUND;

    EM.addComponent<ObjectComponent>(entity);
    auto& o = EM.getComponent<ObjectComponent>(entity.getComponentKey<ObjectComponent>().value());
    EM.addComponent<Render3dComponent>(entity);
    auto& r = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
    r.initRender3dComponent(idObj);                     // ID de referencia para el ManagerTexturas
    EM.addComponent<EventComponent>(entity);

    o.type = PowerUps::ANYONE;
                             
    std::string eventPath = "event:/Efectos/boost-ground";                                           //Ruta del evento
    std::array<const std::string, 5> eventsPaths = {eventPath, "", "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    // Añadir el componente al EntityManager
    EM.addComponent<SoundComponent>(entity);
    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent);     
                                            // Mover el componente al slot correspondiente
    sound.initParent(soundEngine);                                             // Mover el componente al slot correspondiente
    sound.sounds[0].is3D = true;
    sound.sounds[0].shouldPlay = true;

    soundEngine->setEvent3DPosition(sound.sounds[0], {pos.x, pos.z, pos.y}, {0, 0, 1}, {0, 1, 0});

    myVector3 position = {pos.x, pos.y, pos.z};
    float rotationZ = pos.w;
    o.initPhysics(dynamicsWorld, position, myVector3(0.0f, 0.0f, 0.0f), entity.id, "box", idObj, rotationZ, objectRigidType, mass);
    o.rigidBody->setUserPointer(&entity);

    return entity;
}

E& EntityFactory::createEntityCamera(EManager& EM, std::array<myVector3, 12> animCamera){
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::CAMERA;
    EM.addComponent<CameraComponent>(entity);
    EM.getComponent<CameraComponent>(entity.getComponentKey<CameraComponent>().value()).cinematicPos = animCamera;
    EM.getComponent<CameraComponent>(entity.getComponentKey<CameraComponent>().value()).cinematicIt = 1;
    EM.getComponent<CameraComponent>(entity.getComponentKey<CameraComponent>().value()).cinematicRestart = true;

    return entity;
}

E& EntityFactory::createButton(EManager& EM, ISound* soundEngine, int id, int idSprite, int posX, int posY, int sizeX, int sizeY, int alpha)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::BOTON;

    EM.addComponent<Render2dComponent>(entity);
    auto& r = EM.getComponent<Render2dComponent>(entity.getComponentKey<Render2dComponent>().value());
    r.initRender2dComponent(idSprite, posX, posY, alpha);

    EM.addComponent<ButtonComponent>(entity);
    auto& b = EM.getComponent<ButtonComponent>(entity.getComponentKey<ButtonComponent>().value());
    b.initButtonComponent(id,posX, posY, sizeX, sizeY);

    EM.addComponent<SoundComponent>(entity);
    EM.addComponent<EventComponent>(entity);
    std::string eventPath = "event:/Menus/Click";                                            //Ruta del evento
    std::string eventPath1 = "event:/Menus/Hover";
    std::array<const std::string, 5> eventsPaths = {eventPath, eventPath1, "", "", ""};
    auto soundComponent = soundEngine->getSoundComponent(entity.id, eventsPaths);      // Obtener el componente

    auto& sound = EM.getComponent<SoundComponent>(entity.getComponentKey<SoundComponent>().value());
    sound = std::move(soundComponent);
    sound.initParent(soundEngine);

    sound.isButton = true;
    
    return entity;
}

E& EntityFactory::createSprite(EManager& EM, int id, int posX, int posY, int alpha)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::SPRITE;

    EM.addEntityToAux3();

    EM.addComponent<Render2dComponent>(entity);
    auto& r = EM.getComponent<Render2dComponent>(entity.getComponentKey<Render2dComponent>().value());
    r.initRender2dComponent(id, posX, posY, alpha);

    return entity;
}

E& EntityFactory::createText(EManager& EM, std::string text, int posX, int posY, int size, myColor color)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::SPRITE;

    std::string cadena = std::string(text);

    EM.addComponent<RenderTextComponent>(entity);
    auto& r = EM.getComponent<RenderTextComponent>(entity.getComponentKey<RenderTextComponent>().value());
    r.initRenderTextComponent(cadena.c_str(), posX, posY, size, color);

    return entity;
}

E& EntityFactory::createNotAIEntity(EManager& EM, ISound* soundEngine, myVector3 p1, myVector3 p2, myQuaternion ro, uint16_t id)
{
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::NOTAIENTITY;

    EM.addComponent<NotAIComponent>(entity);
    auto& nAI = EM.getComponent<NotAIComponent>(entity.getComponentKey<NotAIComponent>().value());
    nAI.initNotAIComponent(p1, p2, ro);

    EM.addComponent<Render3dComponent>(entity);
    auto& r = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
    r.initRender3dComponent(id);


    return entity;
}

E& EntityFactory::createMuestra(EManager& EM, u_int16_t id){
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::MUESTRA;

    EM.addComponent<Render3dComponent>(entity);
    auto& r = EM.getComponent<Render3dComponent>(entity.getComponentKey<Render3dComponent>().value());
    r.initRender3dComponent(id);

    return entity;
}

E& EntityFactory::createEntityFixedCamera(EManager& EM, myVector3 pos, myVector3 tar){
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::CAMERA;
    EM.addComponent<CameraComponent>(entity);
    EM.getComponent<CameraComponent>(entity.getComponentKey<CameraComponent>().value()).pos = pos;
    EM.getComponent<CameraComponent>(entity.getComponentKey<CameraComponent>().value()).target = tar;
    EM.getComponent<CameraComponent>(entity.getComponentKey<CameraComponent>().value()).fovy = 60.0f;


    return entity;
}

/* ------------------------- CREACION DE PARTICULAS ---------------------------*/
void EntityFactory::particleCreations(EManager& EM, const ParticleTypes pT, int id){

    switch (pT)
    {
    case ParticleTypes::STAR:
            createParticleStar(EM, pT, id);
        break;

    case ParticleTypes::SPARK:
            createParticleSpark(EM, pT, id);
        break;

    case ParticleTypes::CLOUD:
            createParticleCloud(EM, pT, id);
        break;
    }
}

E& EntityFactory::createParticleStar(EManager& EM, const ParticleTypes pT, int idEmitter){
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::PARTICLE;

    EM.addComponent<ParticlesComponent>(entity);
    auto& pc = EM.getComponent<ParticlesComponent>(entity.getComponentKey<ParticlesComponent>().value());
    pc.emitterID = idEmitter;

    auto* emitter = EM.getEntityById(idEmitter);

    pc.type = pT;

    switch (emitter->tipo)
    {
        case EntityType::PLAYER:
        case EntityType::IA:
        case EntityType::PLAYERONLINE:
        {   
            auto& vehicle = EM.getComponent<VehicleComponent>(emitter->getComponentKey<VehicleComponent>().value());
            auto pos = vehicle.m_carChassis->getWorldTransform().getOrigin();

            myVector3 ori = myVector3{
                pos.getX(),
                pos.getY(),
                pos.getZ(),
            };

            pc.initParticle(5, ori, 600, idEmitter);
        } 
            break;

        case EntityType::POWERUP:
        {
            auto& o = EM.getComponent<ObjectComponent>(emitter->getComponentKey<ObjectComponent>().value());

            auto pos = o.rigidBody->getWorldTransform().getOrigin();

            myVector3 ori = myVector3{
                pos.getX(),
                pos.getY(),
                pos.getZ(),
            };

            pc.initParticle(5, ori, 600, idEmitter);
        }
            
            break;
    }   

    pc.init_one_particle();

    return entity;
}

E& EntityFactory::createParticleSpark(EManager& EM, const ParticleTypes pT, int idEmitter){
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::PARTICLE;

    EM.addComponent<ParticlesComponent>(entity);
    auto& pc = EM.getComponent<ParticlesComponent>(entity.getComponentKey<ParticlesComponent>().value());
    pc.emitterID = idEmitter;
    pc.type = pT;

    auto* emitter = EM.getEntityById(idEmitter);


    switch (emitter->tipo)
    {
        case EntityType::PLAYER:
        case EntityType::IA:
        case EntityType::PLAYERONLINE:
        {   
            auto& vehicle = EM.getComponent<VehicleComponent>(emitter->getComponentKey<VehicleComponent>().value());
            auto pos = vehicle.interpolatedTransform.getOrigin();

            myVector3 ori = myVector3{
                pos.getX(),
                pos.getY(),
                pos.getZ(),
            };

            pc.initParticle(10, ori, 600, idEmitter);
        } 
            break;

        case EntityType::POWERUP:
        {
            auto& o = EM.getComponent<ObjectComponent>(emitter->getComponentKey<ObjectComponent>().value());

            auto pos = o.rigidBody->getWorldTransform().getOrigin();

            myVector3 ori = myVector3{
                pos.getX(),
                pos.getY(),
                pos.getZ(),
            };

            pc.initParticle(10, ori, 600, idEmitter);
        }
            
            break;
    }   

    pc.init_one_particle();

    return entity;
}

E& EntityFactory::createParticleCloud(EManager& EM, const ParticleTypes pT, int idEmitter){
    E& entity = EM.CreateEntity();
    entity.tipo = EntityType::PARTICLE;

    EM.addComponent<ParticlesComponent>(entity);
    auto& pc = EM.getComponent<ParticlesComponent>(entity.getComponentKey<ParticlesComponent>().value());
    pc.emitterID = idEmitter;
    pc.type = pT;

    auto* emitter = EM.getEntityById(idEmitter);

    switch (emitter->tipo)
    {
        case EntityType::PLAYER:
        case EntityType::IA:
        case EntityType::PLAYERONLINE:
        {   
            auto& vehicle = EM.getComponent<VehicleComponent>(emitter->getComponentKey<VehicleComponent>().value());
            auto pos = vehicle.interpolatedTransform.getOrigin();

            myVector3 ori = myVector3{
                pos.getX(),
                pos.getY(),
                pos.getZ(),
            };

            pc.initParticle(10, ori, 601, idEmitter);
        } 
            break;

        case EntityType::POWERUP:
        {
            auto& o = EM.getComponent<ObjectComponent>(emitter->getComponentKey<ObjectComponent>().value());

            auto pos = o.rigidBody->getWorldTransform().getOrigin();

            myVector3 ori = myVector3{
                pos.getX(),
                pos.getY(),
                pos.getZ(),
            };

            pc.initParticle(10, ori, 601, idEmitter);
        }
            
            break;
    }   

    pc.init_one_particle();

    return entity;
}
