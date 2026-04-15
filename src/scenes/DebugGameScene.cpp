#include "DebugGameScene.hpp"

#include "../man/GameManager.hpp"

#include "../util/JsonManager.hpp"

#include "../util/CollisionInBroadphase.hpp"
#include "../util/EnumsActions.hpp"
#include "../util/ComponentKey.hpp"

#include <chrono>
#include <random>

// Implementación de DebugGameScene
void DebugGameScene::chargeLevel(LevelType lvlType, std::array<myVector3, 6>& posiciones, std::array<myVector3, 12>& animCinematic){
    std::string pathLevelJson{};

    switch (lvlType) {
        case LevelType::MAPA_JUEGOS:
            pathLevelJson = "assets/JSON/circuits/salonjuguetes.json";
            break;
        case LevelType::MAPA_ASEO:
            pathLevelJson = "assets/JSON/circuits/aseo.json";
            break;
        case LevelType::MAPA_COCINA:
            pathLevelJson = "assets/JSON/circuits/cocina.json";
            break;
        default:
            pathLevelJson = "assets/JSON/circuits/default.json"; // O manejar error
            break;
    }
    std::vector<data> dataMapUnic;
    std::vector<dataDuplicates> dataMapDuplicates;

    dataMapUnic.reserve(50);
    dataMapDuplicates.reserve(50);

    JSONManager manJSON;
    if(manJSON.loadJSON(pathLevelJson)){

        manJSON.loadLevelData(dataMapUnic, dataMapDuplicates);
        vueltasMax = manJSON.getInt("Vueltas");

        for(int i = 0; i < posiciones.size(); i++){
            posiciones[i] = manJSON.getVector3(std::to_string(i+1));
        }

        animCinematic[0] = manJSON.getVector3("InitCamera0Pos");        
        animCinematic[1] = manJSON.getVector3("InitCamera0Tar");
        animCinematic[2] = manJSON.getVector3("PosFinCamera0");        
        animCinematic[3] = manJSON.getVector3("TarFinCamera0");

        animCinematic[4] = manJSON.getVector3("InitCamera1Pos");
        animCinematic[5] = manJSON.getVector3("InitCamera1Tar");
        animCinematic[6] = manJSON.getVector3("PosFinCamera1");
        animCinematic[7] = manJSON.getVector3("TarFinCamera1");

        animCinematic[8] = manJSON.getVector3("InitCamera2Pos");
        animCinematic[9] = manJSON.getVector3("InitCamera2Tar");
        animCinematic[10] = manJSON.getVector3("PosFinCamera2");
        animCinematic[11] = manJSON.getVector3("TarFinCamera2");
    }

    for(const auto& data : dataMapUnic){
        ObjectRigidType type = ObjectRigidType::TYPE_UNKNOW;
        if(data.typeRigidBody == "WALL"){
            type = ObjectRigidType::TYPE_WALL;
            Factory.createObjectNw(EM, world_.get(), {0,0,0}, 0.0f, data.shade, data.id,gameManager.getSoundEngine() ,type);
        }
        if(data.typeRigidBody == "GROUND"){
            type = ObjectRigidType::TYPE_GROUND;
            Factory.createObjectNw(EM, world_.get(), {0,0,0}, 0.0f, data.shade, data.id,gameManager.getSoundEngine() ,type);
        }
        if(data.typeRigidBody == "RENDER"){
            Factory.createEntityOnlyRender(EM, data.id);
        }
        if(data.typeRigidBody == "BOOSTGROUND"){
            type = ObjectRigidType::TYPE_BOOST_GROUND;
            Factory.createObjectNw(EM, world_.get(), {0,0,0}, 0.0f, data.shade, data.id,gameManager.getSoundEngine() ,type);
        }
    }

    for(const auto& data : dataMapDuplicates){
        std::vector<myVector4> positions;
        if(manJSON.loadJSON(data.pathPositions)){
            manJSON.leerposiciones(data.pathPositions, positions); // No debería pasarle el path pero ahora esta mal hecho el leer
        }

        int id = 0;
        if(data.type == types::WAYPOINT){
            for(const auto& pos : positions){
                Factory.createWaypoint(EM, world_.get(), pos, {0,0,0}, id, data.id);
                id++;
            }
        }
        if(data.type == types::BOXPOWERUP){
            for(const auto& pos : positions){
                Factory.createObjectNw(EM, world_.get(), {pos.x, pos.y, pos.z}, pos.w, data.shade, data.id,gameManager.getSoundEngine(), ObjectRigidType::TYPE_BOX_POWER_UP, 0.0f, true);
            }
        }
        if(data.type == types::BOOSTGROUND){
            for(const auto& pos : positions){
                Factory.createBoostGround(EM, world_.get(), gameManager.getSoundEngine(), pos, {0,0,0}, data.id, ObjectRigidType::TYPE_BOOST_GROUND);
                //Factory.createObjectNw(EM, world_.get(), {pos.x, pos.y, pos.z}, data.shade, data.id,gameManager.getSoundEngine(), ObjectRigidType::TYPE_BOOST_GROUND, 0.0f);
            }
        }
        if(data.type == types::WALL){
            for(const auto& pos : positions){
                Factory.createBoostGround(EM, world_.get(), gameManager.getSoundEngine(), pos, {0,0,0}, data.id, ObjectRigidType::TYPE_WALL);
            }
        }
    }

    //Creamos el plano infinito
    Factory.createObjectNw(EM, world_.get(),myVector3(0,0,0), 0.0f, "NoAuto", 0,gameManager.getSoundEngine(), ObjectRigidType::TYPE_GROUND_INFINITY);
    // -------------------------------------------------------
}

void DebugGameScene::init()
{
    gameState = StateGame::RACE;
    initTimer = false;
    timer = 0.0f;
    stage = 0;

    std::array<myVector3, 6> posiciones {};
    std::array<myVector3, 12> animCamera {};
    // -------------Inicializar objetos de bullet-------------
    collisionConfiguration_ = std::make_unique<btDefaultCollisionConfiguration>();
    dispatcher_ = std::make_unique<btCollisionDispatcher>(collisionConfiguration_.get());
    broadphase_ = std::make_unique<btDbvtBroadphase>();
    solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();
    world_ = std::make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(), broadphase_.get(), solver_.get(), collisionConfiguration_.get());
    world_->setGravity(btVector3(0, -10, 0));

    filterCallback = std::make_unique<FilterCallback>(&EM, &EVM);
    if (world_->getPairCache()->getOverlapFilterCallback() == nullptr)
    {
        world_->getPairCache()->setOverlapFilterCallback(filterCallback.get());
    }

    //Debug WARNING JAVIER
    world_->setDebugDrawer(gameManager.getRenderManager()->getDebugWold());

    // -------------------------------------------------------
    
    // -------------Cargador de niveles-------------
    chargeLevel(LevelType::MAPA_JUEGOS, posiciones, animCamera); //(Por ahora estático, deberia venir desde el menú)

    //-------------Creacion de coches y cámara----------------
    JSONManager& JM = gameManager.getJSONManager();
    JM.loadJSON("assets/JSON/playerConfig.json");

    int selectedCar = JM.getInt("PlayerCar");
    
    Factory.CreateVehicle(EM, world_.get(), gameManager.getSoundEngine(), posiciones[5], 5 , selectedCar, "Player");
    Factory.createEntityCamera(EM, animCamera);

    //----------------------------- Creacion IA -----------------------
    // Pool de IDs disponibles
    std::array<int, 15> carPool = {501, 502, 503, 504, 505, 511, 512, 513, 514, 515, 521, 522, 523, 524, 525};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    std::uniform_int_distribution<size_t> carDist(0, carPool.size() - 1);

    //Cargo el JSON de las IA
    JM.loadJSON("assets/JSON/IAconfig.json");
    auto visionDistanceScale = JM.getVector3("VisionDistance");
    auto time2UpdateScale = JM.getVector3("Time2Update");
    auto arrivalRadiusScale = JM.getVector3("ArrivalRadius");
    auto time2ArriveScale = JM.getVector3("Time2Arrive");

    for(int i = 0; i < 3; i++){
        float inteligence = dist01(gen); 
        float crazyness = dist01(gen);

        // Parámetros afectados por inteligencia
        float visionDistance = visionDistanceScale.x + inteligence * (visionDistanceScale.y - visionDistanceScale.x);
        float time2Update = time2UpdateScale.x + inteligence * (time2UpdateScale.y - time2UpdateScale.x);

        // Parámetros afectados Fpor locura
        float arrivalRadius = arrivalRadiusScale.x + crazyness * (arrivalRadiusScale.y - arrivalRadiusScale.x);
        float time2Arrive = time2ArriveScale.x + crazyness * (time2ArriveScale.y - time2ArriveScale.x);
        
        int car = carPool[carDist(gen)];

        Factory.CreateIAVehicle(EM, world_.get(), i + 1, "IA vehicle", gameManager.getSoundEngine(), arrivalRadius, time2Arrive, visionDistance, time2Update, posiciones[i], car);
    }

    gameManager.setResetGameScene(false);

    // Falta por añadir:
    // Sonido
    // Cuando se va a borrar esta entidad event:/Efectos/inicio-carrera
    entityMusicRace = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/carrera-music");
    sfxCountDown = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Efectos/inicio-carrera");
    // Debe hacerlo el propio sistema de sonido (Se cambia el componente de sonido)
    //EM.getComponent<SoundComponent>(entityMusicRace->getComponentKey<SoundComponent>().value()).shouldPlay = true;
    //EM.getComponent<SoundComponent>(sfxCountDown->getComponentKey<SoundComponent>().value()).shouldPlay = true;

    // JSONManager sonidosActivaciones;
    // sonidosActivaciones.loadJSON("assets/JSON/sounds/activaciones-sonidos.json");
    // std::vector<myVector4> posicionesSonidoActivaciones;
    // sonidosActivaciones.leerposiciones("assets/JSON/sounds/activaciones-sonidos.json", posicionesSonidoActivaciones);

    // for( const auto& vec : posicionesSonidoActivaciones)
    // {
    //     myVector4 vec1 = { 0.0f, 0.0f, 0.0f, 0.0f};
    //     Factory.createEntityActivationSound(EM, world_.get(), gameManager.getSoundEngine(), "event:/Ambientes/Castle-Monster", vec, 1000, vec);
    // }
}

void DebugGameScene::update(float deltaTime)
{
    // float deltaTime {1 / 60.0f};

    switch (gameState)
    {
        case StateGame::COUNTDOWN:
            if(!initTimer){
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(sfxCountDown->id, 0));
                //Cargo el HUD
                HUD[0] = &Factory.createSprite(EM, 36, 0, 0, 255);      //3
                HUD[1] = &Factory.createSprite(EM, 37, 0, 0, 0);        //2
                HUD[2] = &Factory.createSprite(EM, 38, 0, 0, 0);        //1
                HUD[3] = &Factory.createSprite(EM, 39, 0, 0, 0);        //Go!

                timer = gameManager.getRenderManager()->getWindowTime();
                initTimer = true;

            }

            updateCountdown(deltaTime);

            if((gameManager.getRenderManager()->getWindowTime()-timer)>3.0f){
                initTimer = false;
                clearHUD();
                EM.comprobarState();
                gameState = StateGame::RACE;
                auto bost = EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value()).boostCharge;
                if(bost > 0.65f && bost < 0.85f){
                    EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value()).gozBoost = true;
                }
            }
            break;

        case StateGame::RACE:
            if(!initTimer){
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(entityMusicRace->id, 0));
                //Cargo el HUD
                HUD[1] = &Factory.createSprite(EM, 20, 48, 44, 0);        //PowerUp Item
                HUD[2] = &Factory.createSprite(EM, 10, 55, 962, 0);       //Vueltas
                HUD[3] = &Factory.createSprite(EM, 13, 1673, 817, 0);     //Posicion

                HUD[0] = &Factory.createText(EM,"no text", 0, 0, 40, {0,0,0,255});

                timer = gameManager.getRenderManager()->getWindowTime();
                initTimer = true;

            }

            updateRace(deltaTime);

            if(EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value()).vueltas == vueltasMax){
                //CAMBIAR EL INPUT COMPONENT POR IA -- POR HACER
                auto* player = EM.getEntityByType(EntityType::PLAYER);
                auto* cam = EM.getEntityByType(EntityType::CAMERA);
                EM.delete_one_component<InputComponent>(*player);
                EM.addComponent<AIComponent>(*player);
                EM.getComponent<AIComponent>(player->getComponentKey<AIComponent>().value()).initAIComponent(0.3f, 0.15f, 7.0f, 0.1f);
                //CAMBIAR CAMARA A ORBITAL
                EM.getComponent<CameraComponent>(cam ->getComponentKey<CameraComponent>().value()).camType = CameraType::ORBITAL;
                //GUARDAR EN EL VEHICLE EL TIEMPO QUE HA TARDADO
                EM.getComponent<VehicleComponent>(player ->getComponentKey<VehicleComponent>().value()).tiempoCarrera = gameManager.getRenderManager()->getWindowTime()-timer;
                initTimer = false;
                clearHUD();
                EM.comprobarState();
                gameState = StateGame::ENDRACE;
            }
            break;

        case StateGame::ENDRACE:
            if(!initTimer){
                timer = gameManager.getRenderManager()->getWindowTime();
                initTimer = true;

                auto* player = EM.getEntityByType(EntityType::PLAYER);
                auto& vc = EM.getComponent<VehicleComponent>(player ->getComponentKey<VehicleComponent>().value());

                int minutos = static_cast<int>(vc.tiempoCarrera) / 60;
                int segundos = static_cast<int>(vc.tiempoCarrera) % 60;
                int milisegundos = static_cast<int>((vc.tiempoCarrera - static_cast<int>(vc.tiempoCarrera)) * 10000); // 4 decimales

                // Formatear el texto
                std::ostringstream oss;
                oss << std::setfill('0') << std::setw(2) << minutos << ":"
                    << std::setw(2) << segundos << ":"
                    << std::setw(4) << milisegundos;

                auto text = oss.str();

                HUD[0] = &Factory.createSprite(EM, 40, 0, 0, 255);                     //PowerUp HUD
                HUD[1] = &Factory.createSprite(EM, 13 + (vc.posicion - 1), 329, 423, 255);                 //PowerUp Item
                HUD[2] = &Factory.createText(EM, text, 800, 500, 95, {0,0,0,255});       //Vueltas
                HUD[3] = &Factory.createSprite(EM, 40, 0, 0, 0);   
            }
            updateEndRace(deltaTime);
            if((gameManager.getRenderManager()->getWindowTime()-timer)>10.0f){
                clearHUD();
                EM.comprobarState();
                cleanup();
                gameManager.changeScene<MainMenuScene>();
            }
            break;
    }
}

void DebugGameScene::update_sim(float simDeltaTime) {

    //const float simDeltaTime {1 / 15.0f};

    Vsystem.savePhysicsPrevious(EM);

    if(gameState == StateGame::RACE){
        Vsystem.update(EM, EVM, simDeltaTime);     
    }

    world_->stepSimulation(simDeltaTime, 4);

    Vsystem.savePhysicsCurrent(EM);
    Osystem.update(EM, simDeltaTime);
    
    gameManager.setLastInput(0);

}

void DebugGameScene::render()
{
    
    Rsystem.update(EM, gameManager.getRenderManager());
}

void DebugGameScene::updateCinematic(float deltaTime){

    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    Ssystem.update(EM, gameManager.getSoundEngine());
    Csystem.update(EM, deltaTime);
}

void DebugGameScene::updateCountdown(float deltaTime){

    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER
    
    Isystem.update(EM, actionInputMask);

    if(EM.getComponent<InputComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<InputComponent>().value()).actionInput.inputMask & GameActions::ACTION_ACCELERATE)
    {
        EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value()).boostCharge += 0.5 * deltaTime;
    }

    Esystem.update(EVM, EM);
    Ssystem.update(EM, gameManager.getSoundEngine());
    Csystem.update(EM, deltaTime);

}


void DebugGameScene::updateRace(float deltaTime){
    //Carrera -------------------------------------------------------
    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER
    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    AIsystem.update(EM, world_.get(), true, deltaTime);
    Isystem.update(EM, actionInputMask);
    Visystem.update(EM, EVM, deltaTime);
    Ssystem.update(EM, gameManager.getSoundEngine());
    CLsystem.update(EM);
    Csystem.update(EM, deltaTime);
    Psystem.update(EM, deltaTime);

    // LOS DATOS QUE HE PUESTO DE LOS SONIDOS SON FIJOS PORQUE ANTES SE COGIAN DEL SCENEMANAGER Y AHORA YA NO ESTAN
    Factory.creationRequestprocess(EM, world_.get(), gameManager.getSoundEngine());
    EM.comprobarClearEntitiesAux1();
    EM.comprobarClearEntitiesAux2();
    EM.comprobarState();

    auto& vc = EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value());
    
    //Actualizo las animaciones del hud
    auto& texto = EM.getComponent<RenderTextComponent>(HUD[0]->getComponentKey<RenderTextComponent>().value());
    auto& powerI = EM.getComponent<Render2dComponent>(HUD[1]->getComponentKey<Render2dComponent>().value());
    auto& lap = EM.getComponent<Render2dComponent>(HUD[2]->getComponentKey<Render2dComponent>().value());
    auto& pos = EM.getComponent<Render2dComponent>(HUD[3]->getComponentKey<Render2dComponent>().value());

    texto.text.texto = "UTILITY\n";

    for (auto& e : EM.getEntitiesAux1()) {
        if(EM.hasComponent<AIComponent>(*EM.getEntityByPos(e))){
            auto& iaComp = EM.getComponent<AIComponent>(EM.getEntityByPos(e)->getComponentKey<AIComponent>().value());
        
            // Obtener la acción actual
            auto currentAction = iaComp.actions[iaComp.actionIt];
            if (currentAction) {
                texto.text.texto += "- " + std::to_string(e) + " -> " + currentAction->getName() + ": " + std::to_string(currentAction->getScore()) + "\n";
            }
        }
    }

   //Fin de la carrera --------------------------------------------------
}

void DebugGameScene::updateEndRace(float deltaTime){
    //Carrera -------------------------------------------------------
    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER
    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    AIsystem.update(EM, world_.get(), true, 1/60); // CAMBIAR
    Ssystem.update(EM, gameManager.getSoundEngine());
    CLsystem.update(EM);
    Csystem.update(EM, deltaTime);

    Factory.creationRequestprocess(EM, world_.get(), gameManager.getSoundEngine());
    EM.comprobarClearEntitiesAux1();
    EM.comprobarClearEntitiesAux2();
    EM.comprobarState();
    //PROVISIONAL
    /*
    gameManager.getRenderManager()->initRender();
    world_->debugDrawWorld();
    gameManager.getRenderManager()->finishRender();
    */
    //Fin de la carrera --------------------------------------------------
}


void DebugGameScene::cleanup()
{
    // O borrar todas los que las entidades tengan componente de sonido
    gameManager.getSoundEngine()->removeInstanceEvent(EM.getComponent<SoundComponent>(entityMusicRace->getComponentKey<SoundComponent>().value()));
    auto* soundEngine = gameManager.getSoundEngine();
    if (soundEngine)
    {
        soundEngine->stopAllEvents();
    }

    Vsystem.remove(EM, world_.get());

    for (int i = world_->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = world_->getCollisionObjectArray()[i];
        world_->removeCollisionObject(obj);
    }

    Ssystem.removeAllEventInstances(EM, soundEngine);

    EM.resetEntityManager();

    world_.reset();
    solver_.reset();
    broadphase_.reset();
    dispatcher_.reset();
    collisionConfiguration_.reset();
}

void DebugGameScene::clearHUD(){
    for(int i = 0; i < HUD.size(); i++){
        HUD[i]->state = false;
        HUD[i] = nullptr;
    }
}


void DebugGameScene::translateInput(){
    myInput actualInput = gameManager.getLastInput();
    actionInputMask.inputMask = 0;
    actionInputMask.R2 = 0;
    actionInputMask.L2 = 0;
    actionInputMask.RJ = 0;
    actionInputMask.LJ = 0;

    if (actualInput.inputMask & ComponentKey::COMPONENT_CTRL){
        actionInputMask.inputMask |= GameActions::ACTION_BACKCAM;
    }
    if (actualInput.inputMask & ComponentKey::COMPONENT_W){
        actionInputMask.inputMask |= GameActions::ACTION_ACCELERATE;
        actionInputMask.R2 = actualInput.R2;
    }
    if (actualInput.inputMask & ComponentKey::COMPONENT_S){
        actionInputMask.inputMask |= GameActions::ACTION_BRAKE;
        actionInputMask.L2 = actualInput.L2;
    }
    if (actualInput.inputMask & ComponentKey::COMPONENT_A){
        actionInputMask.inputMask |= GameActions::ACTION_LEFT;
        actionInputMask.LJ = actualInput.LJ;
    }
    if (actualInput.inputMask & ComponentKey::COMPONENT_D){
        actionInputMask.inputMask |= GameActions::ACTION_RIGHT;
        actionInputMask.LJ = actualInput.LJ;
    }
    if ((actualInput.inputMask & ComponentKey::COMPONENT_SF) || (actualInput.inputMask & ComponentKey::COMPONENT_ENTER)){
        actionInputMask.inputMask |= GameActions::ACTION_POWERUP;
    }
    if (actualInput.inputMask & ComponentKey::COMPONENT_SP){
        actionInputMask.inputMask |= GameActions::ACTION_DRIFT;
    }
}