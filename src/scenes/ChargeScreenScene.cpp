#include "ChargeScreenScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

void ChargeScreenScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
    std::vector<dataSprite> dataMenu {};
    std::array<std::string, 4> buttonData {};
    JSONManager jsonManager;
    jsonManager.loadJSON(path);
    jsonManager.loadMenuItems(dataMenu);
    jsonManager.loadJSON("assets/JSON/renderManager/sprites.json");

    JSONManager jsonButtonManager;
    jsonButtonManager.loadJSON("assets/JSON/renderManager/button.json");
    
    for(const auto& item : dataMenu){
        EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
        
    }
    
    musicMenu = &EF.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/pagina-inical-music");
    gameManager.idMusicMenu = musicMenu->id;

}

void ChargeScreenScene::init()
{    
    loadMenu("assets/JSON/menus/esce.json", EM, Factory);    
}


void ChargeScreenScene::update(float deltaTime)
{
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    ESystem.update(EVM, EM);
}

void ChargeScreenScene::update_sim(float simDeltaTime)
{
    // Actualizar la escena
}

void ChargeScreenScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void ChargeScreenScene::cleanup()
{
    EM.resetEntityManager();
}

