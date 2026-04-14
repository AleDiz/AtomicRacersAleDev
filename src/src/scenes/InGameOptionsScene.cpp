#include "InGameOptionsScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

void InGameOptionsScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
    std::vector<dataSprite> dataMenu {};
    std::array<std::string, 4> buttonData {};
    JSONManager jsonManager;
    jsonManager.loadJSON(path);
    jsonManager.loadMenuItems(dataMenu);
    jsonManager.loadJSON("assets/JSON/renderManager/sprites.json");

    JSONManager jsonButtonManager;
    jsonButtonManager.loadJSON("assets/JSON/renderManager/button.json");
    
    for(const auto& item : dataMenu){
        if(item.button==1){
            int idSprite = jsonButtonManager.getButtonMapping(item.id);
            jsonManager.getInfo2D(idSprite, buttonData);
            EF.createButton(EM, gameManager.getSoundEngine(), item.id, idSprite, item.posX, item.posY, std::stof(buttonData[2]), std::stof(buttonData[3]), item.alpha);
        }else{
            EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
        }
    }
    
    musicMenu = &EF.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/pagina-inical-music");
    gameManager.idMusicMenu = musicMenu->id;
}

void InGameOptionsScene::init()
{    
    loadMenu("assets/JSON/menus/in_ajustes.json", EM, Factory);    
}


void InGameOptionsScene::update(float deltaTime)
{
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
    ESystem.update(EVM, EM);
    Ssystem.update_menus(EM, gameManager.getSoundEngine());
}

void InGameOptionsScene::update_sim(float simDeltaTime)
{
    // Actualizar la escena
}

void InGameOptionsScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void InGameOptionsScene::cleanup()
{
    EM.resetEntityManager();
}

