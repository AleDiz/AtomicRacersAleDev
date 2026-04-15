#include "GameScene.hpp"

#include "../man/GameManager.hpp"

#include "../util/JsonManager.hpp"

#include "../util/CollisionInBroadphase.hpp"
#include "../util/EnumsActions.hpp"
#include "../util/ComponentKey.hpp"

#include <chrono>

// Implementación de GameScene
void GameScene::chargeLevel(LevelType lvlType, std::array<myVector3, 6>& posiciones, std::array<myVector3, 12>& animCinematic){
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
        if(data.typeRigidBody == "TYPE_WALL_SPECIAL"){
            type = ObjectRigidType::TYPE_WALL_SPECIAL;
            Factory.createObjectNw(EM, world_.get(), {0,0,0}, 0.0f, data.shade, data.id,gameManager.getSoundEngine() ,type);
        }
        if(data.typeRigidBody == "GROUND"){
            type = ObjectRigidType::TYPE_GROUND;
            Factory.createObjectNw(EM, world_.get(), {0,0,0}, 0.0f, data.shade, data.id,gameManager.getSoundEngine() ,type);
        }
        if(data.typeRigidBody == "RENDER"){
            Factory.createEntityOnlyRender(EM, data.id);
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
        if(data.type == types::TYPE_WALL_SPECIAL){
            for(const auto& pos : positions){
                Factory.createBoostGround(EM, world_.get(), gameManager.getSoundEngine(), pos, {0,0,0}, data.id, ObjectRigidType::TYPE_WALL_SPECIAL);
            }
        }
        
    }

    //Creamos el plano infinito
    Factory.createObjectNw(EM, world_.get(),myVector3(0,0,0), 0.0f, "NoAuto", 0,gameManager.getSoundEngine(), ObjectRigidType::TYPE_GROUND_INFINITY);
    // -------------------------------------------------------
}

void GameScene::initGame()
{
    std::array<myVector3, 6> posiciones {};
    std::array<myVector3, 12> animCamera {};


    // Colisiones para objetos rotados o mesh con agujero
    // BROADPHASE calcula una caja de colisión de un extremo a otro sin importar lo anterior
    // NARROWPHASE calcula la colisión entre los objetos que han colisionado en el broadphase
    gContactAddedCallback = [](btManifoldPoint& cp,
                        const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,
                        const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1) 
    {
        auto* obj0 = const_cast<btCollisionObject*>(colObj0Wrap->getCollisionObject());
        auto* obj1 = const_cast<btCollisionObject*>(colObj1Wrap->getCollisionObject());

        int type0 = obj0->getUserIndex();
        int type1 = obj1->getUserIndex();


        // Solo reaccionar si hay coche y pared involucrados
        if((type0 == ObjectRigidType::TYPE_CHASIS_CAR && type1 == ObjectRigidType::TYPE_WALL_SPECIAL) || (type0 == ObjectRigidType::TYPE_CHASIS_CAR && type1 == ObjectRigidType::TYPE_WALL) )
        {
            obj0->setUserIndex2(1);
        }
        else if((type1 == ObjectRigidType::TYPE_CHASIS_CAR && type0 == ObjectRigidType::TYPE_WALL_SPECIAL) || (type1 == ObjectRigidType::TYPE_CHASIS_CAR && type0 == ObjectRigidType::TYPE_WALL) )
        {
            obj1->setUserIndex2(1);
        }

        if(type0 == ObjectRigidType::TYPE_POWER_UP_RANDOM && type1 == ObjectRigidType::TYPE_WALL_SPECIAL)
        {
            obj0->setUserIndex2(1);
        }
        else if(type1 == ObjectRigidType::TYPE_POWER_UP_RANDOM && type0 == ObjectRigidType::TYPE_WALL_SPECIAL)
        {
            obj1->setUserIndex2(1);
        }

        
        return true; // devuelve false para que Bullet procese el contacto normalmente
    };

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
    
    Factory.CreateVehicle(EM, world_.get(), gameManager.getSoundEngine(), posiciones[5], 1 , selectedCar, "Player");
    // Factory.createNotAIEntity(EM, gameManager.getSoundEngine(), myVector3{260, 1,  -138}, myVector3{260, 1,  -200}, myQuaternion{0,0,0,0}, selectedCar);
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

        // Parámetros afectados por locura
        float arrivalRadius = arrivalRadiusScale.x + crazyness * (arrivalRadiusScale.y - arrivalRadiusScale.x);
        float time2Arrive = time2ArriveScale.x + crazyness * (time2ArriveScale.y - time2ArriveScale.x);

        int car = carPool[carDist(gen)];

        Factory.CreateIAVehicle(EM, world_.get(), i + 1, "IA vehicle", gameManager.getSoundEngine(), arrivalRadius, time2Arrive, visionDistance, time2Update, posiciones[i], car);
    }

    gameManager.setResetGameScene(false);

    // Falta por añadir:
    // Sonido
    // Cuando se va a borrar esta entidad event:/Efectos/inicio-carrera

    JSONManager sonidosGrada;
    sonidosGrada.loadJSON("assets/JSON/sounds/posicionesSonidoGrada.json");
    std::vector<myVector4> posicionesSonidoGrada;
    sonidosGrada.leerposiciones("assets/JSON/sounds/posicionesSonidoGrada.json", posicionesSonidoGrada);
    for( const auto& vec : posicionesSonidoGrada)
    {
        Factory.createEntitySound3D(EM, gameManager.getSoundEngine(), "event:/Efectos/Grada", vec);
    }

    JSONManager sonidosActivaciones;
    sonidosActivaciones.loadJSON("assets/JSON/sounds/activaciones-sonidos.json");
    std::vector<myVector4> posicionesSonidoActivaciones;
    sonidosActivaciones.leerposiciones("assets/JSON/sounds/activaciones-sonidos.json", posicionesSonidoActivaciones);
    for( const auto& vec : posicionesSonidoActivaciones)
    {
        Factory.createEntityActivationSound(EM, world_.get(), gameManager.getSoundEngine(), "event:/Ambientes/Castle-Monster", vec, 1000, vec);
    }
    
    // Debe hacerlo el propio sistema de sonido (Se cambia el componente de sonido)
    //EM.getComponent<SoundComponent>(entityMusicRace->getComponentKey<SoundComponent>().value()).shouldPlay = true;
    //EM.getComponent<SoundComponent>(sfxCountDown->getComponentKey<SoundComponent>().value()).shouldPlay = true;
}


void GameScene::init()
{
    if(gameManager.getResetGameScene())
    {
        gameManager.getSoundEngine()->setPlayBus("bus:/SFX");
        gameState = StateGame::LOADINGSCREEN;
        initTimer = false;
        firstRender = false;
        initializatedGame = false;
        timer = 0.0f;
        stage = 0; 
    }
    else
    {
        gameState = StateGame::RACE;
        gameManager.getSoundEngine()->setPlayBus("bus:/SFX");
    }

}

void GameScene::update(float deltaTime)
{
    //float deltaTime {1 / 60.0f};
    switch (gameState)
    {
        case StateGame::LOADINGSCREEN:

            if(!initTimer){
                timer = gameManager.getRenderManager()->getWindowTime();
                initTimer = true;
                //Cargo el menu de espera con los controles
                Factory.createSprite(EM, 29, 0, 0, 255);        
                Factory.createSprite(EM, 83, 607, 1, 255);       
                Factory.createSprite(EM, 82, 237, 364, 255); 
                Factory.createSprite(EM, 85, 0, 0, 0);
            }
            
            if(firstRender && !initializatedGame)
            {
                initGame();
                auto& sprites = EM.getEntitiesAux3();
                auto& spr1 = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[3])->getComponentKey<Render2dComponent>().value());
                spr1.color.a = 255;
                initializatedGame = true;
            }

            
            if((gameManager.getLastInput().inputMask != 0) && initializatedGame){
                clearHUD();

                initTimer = false;
                EM.comprobarState();

                entityMusicRace = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/carrera-music");
                entityMusicIntro = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/intro-race");
                sfxCountDown = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Efectos/inicio-carrera");
                
                gameState = StateGame::CINEMATIC;
            }
            
            break;

        case StateGame::CINEMATIC:
            if(!initTimer){
                timer = gameManager.getRenderManager()->getWindowTime();
                initTimer = true;
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(entityMusicIntro->id, 0));
            }
            updateCinematic(deltaTime);
            if((gameManager.getRenderManager()->getWindowTime()-timer)>9.0f){
                initTimer = false;
                clearHUD();
                EM.comprobarState();
                gameState = StateGame::COUNTDOWN;
            }

            break;

        case StateGame::COUNTDOWN:
            if(!initTimer){
                EVM.addEvent(EventTypes::STOPSOUND, std::make_unique<StopSoundData>(entityMusicIntro->id, 0));
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(sfxCountDown->id, 0));
                //Cargo el HUD
                Factory.createSprite(EM, 36, 0, 0, 255);      //3
                Factory.createSprite(EM, 37, 0, 0, 0);        //2
                Factory.createSprite(EM, 38, 0, 0, 0);        //1
                Factory.createSprite(EM, 39, 0, 0, 0);        //Go!
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
                
                //EVM.addEvent(EventTypes::STOPSOUND, std::make_unique<StopSoundData>(entityMusicIntro->id, 0));
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(entityMusicRace->id, 0));
                //Cargo el HUD
                Factory.createSprite(EM, 19, 48, 44, 0);        //PowerUp HUD
                Factory.createSprite(EM, 20, 48, 44, 0);        //PowerUp Item
                Factory.createSprite(EM, 10, 55, 962, 0);       //Vueltas
                Factory.createSprite(EM, 13, 1673, 817, 0);     //Posicion
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

                Factory.createSprite(EM, 40, 0, 0, 255);                     
                Factory.createSprite(EM, 13 + (vc.posicion - 1), 596, 423, 255);                 

                JSONManager manJSON;
                manJSON.loadJSON("assets/JSON/player/state.json");
                int monedas = manJSON.getInt("monedas");

                if(vc.posicion == 1){
                    Factory.createSprite(EM, 88, 975, 500, 255); 
                    Factory.createSprite(EM, 86, 1042, 500, 255);     
                    Factory.createSprite(EM, 100,1112, 484, 255); 
                    manJSON.setInt("monedas", monedas + 20);
                }else if(vc.posicion == 2){
                    Factory.createSprite(EM, 87, 975, 500, 255); 
                    Factory.createSprite(EM, 86, 1042, 500, 255);    
                    Factory.createSprite(EM, 100, 1112, 484, 255); 
                    manJSON.setInt("monedas", monedas + 10);
                }else if(vc.posicion == 3){
                    Factory.createSprite(EM, 91, 1042, 500, 255); 
                    Factory.createSprite(EM, 100, 1112, 484, 255); 
                    manJSON.setInt("monedas", monedas + 5);
                }

                manJSON.saveJSON("assets/JSON/player/state.json");
            }
            updateEndRace(deltaTime);
            if((gameManager.getRenderManager()->getWindowTime()-timer)>5.0f){
                clearHUD();
                EM.comprobarState();
                gameManager.changeScene<MainMenuScene>();
            }
            break;
    }
}

void GameScene::update_sim(float simDeltaTime) {

    //const float simDeltaTime {1 / 15.0f};
    
    if(gameState != StateGame::LOADINGSCREEN)
    {
        Vsystem.savePhysicsPrevious(EM);
        Osystem.savePhysicsPrevious(EM);

        if(gameState == StateGame::RACE || gameState == StateGame::ENDRACE){
            Vsystem.update(EM, EVM, simDeltaTime);     
        }

        world_->stepSimulation(simDeltaTime, 4);

        Osystem.update(EM, simDeltaTime);
    
        Vsystem.savePhysicsCurrent(EM);
        Osystem.savePhysicsCurrent(EM);
        
        gameManager.setLastInput(0);
    }

}

void GameScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
    if(!firstRender)
        firstRender = true;
}

void GameScene::updateCinematic(float deltaTime)
{
    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Osystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    Ssystem.update(EM, gameManager.getSoundEngine());
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);

}

void GameScene::updateCountdown(float deltaTime){

    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER
    
    Isystem.update(EM, actionInputMask);

    if(EM.getComponent<InputComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<InputComponent>().value()).actionInput.inputMask & GameActions::ACTION_ACCELERATE)
    {
        EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value()).boostCharge += 0.5 * deltaTime;
    }

    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Osystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    Ssystem.update(EM, gameManager.getSoundEngine());
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);

    auto& sprites = EM.getEntitiesAux3();
    
    //Actualizo el HUD
    auto& spr3 = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[0])->getComponentKey<Render2dComponent>().value());
    auto& spr2 = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[1])->getComponentKey<Render2dComponent>().value());
    auto& spr1 = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[2])->getComponentKey<Render2dComponent>().value());
    auto& go   = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[3])->getComponentKey<Render2dComponent>().value());

    const float fadeSpeed = 255.0f; // Para que desaparezca en 1 segundo
    
    switch (stage) {
        case 0:
            spr3.color.a -= fadeSpeed * deltaTime;
            if (spr3.color.a <= 0) {
                spr3.color.a = 0;
                spr2.color.a = 255;
                stage = 1;
            }
            break;
        case 1:
            spr2.color.a -= fadeSpeed * deltaTime;
            if (spr2.color.a <= 0) {
                spr2.color.a = 0;
                spr1.color.a = 255;
                stage = 2;
            }
            break;
        case 2:
            spr1.color.a -= fadeSpeed * deltaTime;
            if (spr1.color.a <= 0) {
                spr1.color.a = 0;
                go.color.a = 255;
            }
            break;
        }
}


void GameScene::updateRace(float deltaTime)
{
    //Carrera -------------------------------------------------------
    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER
    if(actionInputMask.inputMask & GameActions::ACTION_PAUSE)
    {
        // Parar sonidos de todas las entidades
        gameManager.getSoundEngine()->setPauseBus("bus:/SFX");
        gameManager.changeScene<InGameOptionsScene>();
    }
    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Osystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    AIsystem.update(EM, world_.get(), true, deltaTime);
    Isystem.update(EM, actionInputMask);
    Visystem.update(EM, EVM, deltaTime);
    Ssystem.update(EM, gameManager.getSoundEngine());
    CLsystem.update(EM);
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);
    Psystem.update(EM, deltaTime);


    // LOS DATOS QUE HE PUESTO DE LOS SONIDOS SON FIJOS PORQUE ANTES SE COGIAN DEL SCENEMANAGER Y AHORA YA NO ESTAN
    Factory.creationRequestprocess(EM, world_.get(), gameManager.getSoundEngine());
    EM.comprobarClearEntitiesAux1();
    EM.comprobarState();

    auto& vc = EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value());
    
    auto& sprites = EM.getEntitiesAux3();

    //Actualizo las animaciones del hud
    auto& powerB = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[0])->getComponentKey<Render2dComponent>().value());
    auto& powerI = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[1])->getComponentKey<Render2dComponent>().value());
    auto& lap    = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[2])->getComponentKey<Render2dComponent>().value());
    auto& pos    = EM.getComponent<Render2dComponent>(EM.getEntityByPos(sprites[3])->getComponentKey<Render2dComponent>().value());

    if(vc.powerUp != PowerUps::ANYONE && vc.tamPowerUp >= 0){
        if (static_cast<int>(powerB.color.a) != 255) {
            powerB.color.a += 300 * deltaTime;
            if(powerB.color.a > 255){
                powerB.color.a = 255.0f;
            }
        }
        int tam = std::min(vc.tamPowerUp - 1, 2);
        if(vc.powerUp == PowerUps::BANANA){
            powerI.id = 26 + tam;
        }else if(vc.powerUp == PowerUps::BOOST){
            powerI.id = 20 + tam;
        }else if(vc.powerUp == PowerUps::SHELL){
            powerI.id = 23 + tam;
        }
        if (static_cast<int>(powerI.color.a) != 255) {
            powerI.color.a += 300*deltaTime;
            if(powerI.color.a > 255){
                powerI.color.a = 255.0f;
            }
        }      
    }else if(powerB.color.a != 0){
        powerB.color.a -= 300 * deltaTime;
        if(powerB.color.a < 0){
            powerB.color.a = 0;
        }
        powerI.color.a = 0;
    }

    if (static_cast<int>(lap.color.a) != 255) {
        lap.color.a += 300*deltaTime;
        if(lap.color.a > 255){
            lap.color.a = 255.0f;
        }
    }else{
        lap.id = 10 + (vc.vueltas);
    }

    if (static_cast<int>(pos.color.a) != 255) {
        pos.color.a += 300*deltaTime;
        if(pos.color.a > 255){
            pos.color.a = 255.0f;
        }
    }else{
        pos.id = 13 + (vc.posicion - 1);
    }

    //PROVISIONAL
    /*
    gameManager.getRenderManager()->initRender();
    world_->debugDrawWorld();
    gameManager.getRenderManager()->finishRender3D();
    gameManager.getRenderManager()->finishRender();
    */
   //Fin de la carrera --------------------------------------------------
}

void GameScene::updateEndRace(float deltaTime){
    //Carrera -------------------------------------------------------
    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER

    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Osystem.interpolate(EM, gameManager.getInterpolationFactor());

    Esystem.update(EVM, EM);
    AIsystem.update(EM, world_.get(), true, deltaTime); // CAMBIAR
    Ssystem.update(EM, gameManager.getSoundEngine());
    CLsystem.update(EM);
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);
    Psystem.update(EM, deltaTime);

    Factory.creationRequestprocess(EM, world_.get(), gameManager.getSoundEngine());
    EM.comprobarClearEntitiesAux1();
    EM.comprobarState();
    //PROVISIONAL
    /*
    gameManager.getRenderManager()->initRender();
    world_->debugDrawWorld();
    gameManager.getRenderManager()->finishRender();
    */
    //Fin de la carrera --------------------------------------------------
}


void GameScene::cleanup()
{
    // O borrar todas los que las entidades tengan componente de sonido
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

    gameState = StateGame::LOADINGSCREEN;
    gameManager.setResetGameScene(true);

}

void GameScene::clearHUD(){

    for(auto& HUD : EM.getEntitiesAux3()){

        auto* e = EM.getEntityByPos(HUD);
        e->state = false;
    }

    EM.getEntitiesAux3().clear();
}


void GameScene::translateInput(){
    myInput actualInput = gameManager.getLastInput();
    actionInputMask.inputMask = 0;
    actionInputMask.L2 = 0;
    actionInputMask.LJ = 0;
    actionInputMask.R2 = 0;
    actionInputMask.RJ = 0;

    
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
    if (actualInput.inputMask & ComponentKey::COMPONENT_ESC){
        actionInputMask.inputMask |= GameActions::ACTION_PAUSE;
    }
}