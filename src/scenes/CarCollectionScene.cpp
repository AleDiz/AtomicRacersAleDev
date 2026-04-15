#include "CarCollectionScene.hpp"

#include "../man/GameManager.hpp"
// Implementación de CarCollectionScene

void CarCollectionScene::loadCollection(std::string path, EManager& EM, EntityFactory& EF){
    std::vector<dataSprite> dataMenu {};
    std::array<std::string, 4> buttonData {};
    JSONManager jsonManager;
    jsonManager.loadJSON(path);
    jsonManager.loadMenuItems(dataMenu);
    jsonManager.loadJSON("assets/JSON/renderManager/sprites.json");

    JSONManager jsonButtonManager;
    jsonButtonManager.loadJSON("assets/JSON/renderManager/button.json");
    
    JSONManager& JM = gameManager.getJSONManager();
    JM.loadJSON("assets/JSON/player/state.json");
    std::vector<int> unlockedCars = JM.getUnlockCars();
    
    int i = 0;
    int alpha = 0;
    for(const auto& item : dataMenu){
        if(item.button==1){
            if(std::find(unlockedCars.begin(), unlockedCars.end(), item.id) != unlockedCars.end() || item.id == 12 || item.id < 10){
                alpha = 255;
            }else{
                alpha = 0;
            }
            int idSprite = jsonButtonManager.getButtonMapping(item.id);
            jsonManager.getInfo2D(idSprite, buttonData);
            buttons[i] = &EF.createButton(EM, gameManager.getSoundEngine(), item.id, idSprite, item.posX, item.posY, std::stof(buttonData[2]), std::stof(buttonData[3]), alpha);
            i++;
        }else{
            EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
        }
    }
}

void CarCollectionScene::init()
{    
    loadCollection("assets/JSON/menus/col_car.json", EM, Factory);
    
    //JsonManager -> get ids desloqueadas
    //for que pone los botones con ids en el array alpha->255

    JSONManager& JM = gameManager.getJSONManager();
    JM.loadJSON("assets/JSON/playerConfig.json");
    int coche = JM.getInt("PlayerCar");

    Factory.createMuestra(EM, coche);
    Factory.createEntityFixedCamera(EM, {10.0f, 1.0f, -5.0f}, {0.0f,0.0f,7.0f});
    detalle = &Factory.createSprite(EM, 32, 1005, 675, 0);
}

void CarCollectionScene::update(float deltaTime)
{
    //WARNING ALE
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
    ESystem.update(EVM, EM);
    Ssystem.update_menus(EM, gameManager.getSoundEngine());

    JSONManager& JM = gameManager.getJSONManager();
    JM.loadJSON("assets/JSON/playerConfig.json");
    int coche = JM.getInt("PlayerCar");

    if(coche != 0){
        EM.getComponent<Render3dComponent>(EM.getEntityByType(EntityType::MUESTRA)->getComponentKey<Render3dComponent>().value()).id = coche;

        auto& rDetalle = EM.getComponent<Render2dComponent>(detalle->getComponentKey<Render2dComponent>().value());
        if(coche < 510){            //Coches pequeños 500 - 510
            rDetalle.id = 32;
        }else if(coche >= 520){     //Coches grande 520 - 530
            rDetalle.id = 34;
        }else{                  //Coches medianos 510 - 520
            rDetalle.id = 33;
        }
        rDetalle.color.a = 255;
    }

    
}

void CarCollectionScene::update_sim(float simDeltaTime)
{
    
}

void CarCollectionScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void CarCollectionScene::cleanup()
{
    EM.resetEntityManager();
}