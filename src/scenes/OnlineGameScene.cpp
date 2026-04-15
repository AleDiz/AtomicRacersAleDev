#include "OnlineGameScene.hpp"

#include "../man/GameManager.hpp"

#include "../util/JsonManager.hpp"

#include "../util/CollisionInBroadphase.hpp"
#include "../util/EnumsActions.hpp"
#include "../util/ComponentKey.hpp"

#include <chrono>

void OnlineGameScene::clearHUD(){

    for(auto& HUD : EM.getEntitiesAux3()){

        auto* e = EM.getEntityByPos(HUD);
        e->state = false;
    }

    EM.getEntitiesAux3().clear(); 
}

void OnlineGameScene::chargeLevel(LevelType lvlType, std::array<myVector3, 6>& posiciones, std::array<myVector3, 12>& animCinematic){
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

void OnlineGameScene::init()
{
    timer = 0.0f;
    timer2 = 0.0f;
    initTimer = false;
    initTimer2 = false;
    timer2Expulse = false;
    endRace = false;
    gameState = StateGame::CINEMATIC;
    stage = 0;

    Factory.createSprite(EM, 29, 0, 0, 255);        
    Factory.createSprite(EM, 83, 607, 1, 255);    
    Factory.createSprite(EM, 41, 37, 384, 255); 
    Factory.createSprite(EM, 82, 237, 334, 255);  

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

    Msystem.initConnection(NE);
}

void OnlineGameScene::update(float deltaTime)
{
    //float const deltaTime {1 / 60.0f};
    
    switch (NE.getStateRace())
    {
    case WAITING:
        // RENDERIZAR EL CONJUNTO DE COCHES QUE SE VAN UNIENDO A LA CARRERA (ES UNA IMAGEN DE CADA COCHE Y EL NOMBRE DEL JUGADOR)
        
        Msystem.waitPlayersUpdate(EM, NE);
        break;

    case CHANGETOGAME:
    {
        EM.deleteSlotmaps();
        EM.deleteEntities();
        // -------------------------------------------------------
        std::array<myVector3, 6> posiciones {};
        std::array<myVector3, 12> animCamera {};
        // -------------Cargador de niveles-------------
        chargeLevel(LevelType::MAPA_JUEGOS, posiciones, animCamera); //(Por ahora estático, deberia venir desde el menú)

        gameManager.setResetGameScene(false);
        
        // CREAMOS LAS ENTIDADES CON MULTIPLAYERSYSTEM
        Msystem.sendToCreatePlayers(EM, NE);

        // PROCESAMOS LA CREACION CON ENTITFACTORY
        Factory.creationOnlineRequestprocess(EM, NE, world_.get(), gameManager.getSoundEngine());

        // SINCRONIZAMOS IDS CON MULTIPLAYERSYSTEM
        Msystem.syncPlayersID(EM, NE);

        // ENVIAR PRIMER SYNC AL SERVIDOR
        Msystem.gameUpdateOUT(EM, NE);

        // Cambiamos a la panatalla de carga
        Msystem.setStateRace(NE, OnlineStateRace::CHARGESCREEN);


        Factory.createEntityCamera(EM, animCamera);
        
        // Falta por añadir:
        // Sonido
        // Cuando se va a borrar esta entidad event:/Efectos/inicio-carrera

        changeSprites = true;
        // Debe hacerlo el propio sistema de sonido (Se cambia el componente de sonido)
        //EM.getComponent<SoundComponent>(entityMusicRace->getComponentKey<SoundComponent>().value()).shouldPlay = true;
        //EM.getComponent<SoundComponent>(sfxCountDown->getComponentKey<SoundComponent>().value()).shouldPlay = true;
        break;
    }

    case CHARGESCREEN:
        {
            Msystem.waitPlayersUpdate(EM, NE);
            break;
        }
    case GAME:
        switch (gameState)
        {
            clearHUD();
            
            case StateGame::CINEMATIC:
            {

                if(!initTimer){
                    
                    timer = gameManager.getRenderManager()->getWindowTime();
                    initTimer = true;

                    entityMusicRace = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/carrera-music");
                    entityMusicIntro = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/intro-race");
                    sfxCountDown = &Factory.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Efectos/inicio-carrera");

                    EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(entityMusicIntro->id, 0));
                }   
                updateCinematic(deltaTime);
                if((gameManager.getRenderManager()->getWindowTime()-timer)>9.0f){
                    initTimer = false;
                    EM.comprobarState();
                    gameState = StateGame::COUNTDOWN;
                }
                break;
            }

            case StateGame::COUNTDOWN:
                if(!initTimer){
                    EM.comprobarClearEntitiesAux1();
                    EM.comprobarClearEntitiesAux2();
                    EM.comprobarState();
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
                    EM.comprobarClearEntitiesAux1();
                    EM.comprobarClearEntitiesAux2();
                    EM.comprobarState();
                    EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(entityMusicRace->id, 0));
                    //Cargo el HUD
                    Factory.createSprite(EM, 19, 48, 44, 0);        //PowerUp HUD
                    Factory.createSprite(EM, 20, 48, 44, 0);        //PowerUp Item
                    Factory.createSprite(EM, 10, 55, 962, 0);       //Vueltas
                    Factory.createSprite(EM, 13, 1673, 817, 0);     //Posicion
                    timer = gameManager.getRenderManager()->getWindowTime();
                    initTimer = true;
                    Msystem.initGame(NE);
                }

                updateRace(deltaTime);

                for(auto& e : EM.getEntitiesAux1())
                {
                    if(EM.getComponent<VehicleComponent>(EM.getEntityByPos(e)->getComponentKey<VehicleComponent>().value()).posicion == 1)
                    {
                        if(EM.getComponent<VehicleComponent>(EM.getEntityByPos(e)->getComponentKey<VehicleComponent>().value()).vueltas == vueltasMax)
                        {
                            if(!timer2Expulse)
                            {
                                timerPrimero = gameManager.getRenderManager()->getWindowTime();
                                timer2Expulse = true;
                            }
                        }
                    }

                }

                if(timer2Expulse)
                {
                    if((gameManager.getRenderManager()->getWindowTime()-timerPrimero)>90.0f)
                    {
                        Msystem.disconnect(NE);
                        gameManager.changeScene<MainMenuScene>();
                        endRace = true;
                    }
                }

                if(!endRace){
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
                }

                break;

            case StateGame::ENDRACE:
                if(!initTimer){
                    EM.comprobarClearEntitiesAux1();
                    EM.comprobarClearEntitiesAux2();
                    EM.comprobarState();
                    
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
                }
                updateEndRace(deltaTime);

                if(!endRace)
                {
                    for(auto& e : EM.getEntitiesAux1())
                    {
                        if(EM.getComponent<VehicleComponent>(EM.getEntityByPos(e)->getComponentKey<VehicleComponent>().value()).posicion ==
                            EM.getEntitiesAux1().size())
                        {
                            if(EM.getComponent<VehicleComponent>(EM.getEntityByPos(e)->getComponentKey<VehicleComponent>().value()).vueltas == vueltasMax)
                            {
                                endRace = true;
                            }
                        }
                    }
                }

                if((gameManager.getRenderManager()->getWindowTime()-timer)>90.0f)
                {
                    Msystem.disconnect(NE);
                    gameManager.changeScene<MainMenuScene>();
                    EM.comprobarState();
                }

                if(endRace)
                {
                    if(!initTimer2)
                    {
                        timer2 = gameManager.getRenderManager()->getWindowTime();
                        initTimer2 = true;
                    }

                    if((gameManager.getRenderManager()->getWindowTime()-timer2)>10.0f)
                    {

                        Msystem.disconnect(NE);
                        gameManager.changeScene<MainMenuScene>();
                        EM.comprobarState();
                    }
                }
                break;
        }

        break;
    }
}

void OnlineGameScene::update_sim(float simDeltaTime) {

    //const float simDeltaTime {1 / 15.0f};

    Vsystem.savePhysicsPrevious(EM);
    Osystem.savePhysicsPrevious(EM);

    if(gameState == StateGame::RACE){
        Vsystem.update(EM, EVM, simDeltaTime);     
    }

    world_->stepSimulation(simDeltaTime, 4);

    Osystem.update(EM, simDeltaTime);

    Vsystem.savePhysicsCurrent(EM);
    Osystem.savePhysicsCurrent(EM);
    
    gameManager.setLastInput(0);

}

void OnlineGameScene::updateCinematic(float deltaTime){

    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Osystem.interpolate(EM, gameManager.getInterpolationFactor());
    Esystem.update(EVM, EM);
    Ssystem.update(EM, gameManager.getSoundEngine());
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);
    Psystem.update(EM, deltaTime);
}

void OnlineGameScene::render()
{   
    Rsystem.update(EM, gameManager.getRenderManager());
}

void OnlineGameScene::updateCountdown(float deltaTime){

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
    Psystem.update(EM, deltaTime);
    
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


void OnlineGameScene::updateRace(float deltaTime){
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

    //  FUNCION DE UPDATE DEL INPUT
    Isystem.update(EM, actionInputMask);

    //  ENVIAMOS NUESTROS DATOS AL SERVIDOR
    Msystem.gameUpdateOUT(EM, NE);

    //  PROCESAMOS LAS ENTRADAS (1 FRAME = INPUTS, 4 FRAME = SYNC) (GUARDAR INFO EN MULTIPLAYER COMPONENT)
    Msystem.gameUpdateIN(EM, NE);

    //  FUNCION DE UPDATE DEL INPUT
    Isystem.update(EM, actionInputMask);

    //  FUNCION  DE SYNC DEL VEHICLE SYSTEM
    Vsystem.syncDataSaves(EM);
    Esystem.update(EVM, EM);
    AIsystem.update(EM, world_.get(), true, deltaTime);
    Visystem.update(EM, EVM, deltaTime);
    Ssystem.update(EM, gameManager.getSoundEngine());
    CLsystem.update(EM);
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);
    Psystem.update(EM, deltaTime);

    // LOS DATOS QUE HE PUESTO DE LOS SONIDOS SON FIJOS PORQUE ANTES SE COGIAN DEL SCENEMANAGER Y AHORA YA NO ESTAN
    Factory.creationRequestprocess(EM, world_.get(), gameManager.getSoundEngine());
    EM.comprobarClearEntitiesAux1();
    EM.comprobarClearEntitiesAux2();
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

void OnlineGameScene::updateEndRace(float deltaTime){
    //Carrera -------------------------------------------------------
    translateInput(); //TRADUCIMOS LOS EVENTOS DE TECLADO DEL GAMEMANAGGER
    Vsystem.interpolate(EM, gameManager.getInterpolationFactor());
    Osystem.interpolate(EM, gameManager.getInterpolationFactor());


    //  FUNCION DE UPDATE DEL INPUT
    Isystem.update(EM, actionInputMask);

    //  ENVIAMOS NUESTROS DATOS AL SERVIDOR
    Msystem.gameUpdateOUT(EM, NE);

    //  PROCESAMOS LAS ENTRADAS (1 FRAME = INPUTS, 4 FRAME = SYNC) (GUARDAR INFO EN MULTIPLAYER COMPONENT)
    Msystem.gameUpdateIN(EM, NE);

    //  FUNCION DE UPDATE DEL INPUT
    Isystem.update(EM, actionInputMask);

    //  FUNCION  DE SYNC DEL VEHICLE SYSTEM
    Vsystem.syncDataSaves(EM);
    Esystem.update(EVM, EM);
    AIsystem.update(EM, world_.get(), true, 1/60); // CAMBIAR
    Ssystem.update(EM, gameManager.getSoundEngine());
    CLsystem.update(EM);
    Csystem.update(EM, deltaTime);
    NAIsystem.update(EM, deltaTime);
    Psystem.update(EM, deltaTime);

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


void OnlineGameScene::cleanup()
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

    gameState = StateGame::CINEMATIC;
    gameManager.setResetGameScene(true);
}

void OnlineGameScene::translateInput(){
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