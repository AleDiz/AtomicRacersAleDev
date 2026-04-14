#include "MainMenuScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

void MainMenuScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
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

// Implementación de MainMenuScene
void MainMenuScene::init()
{
    loadMenu("assets/JSON/menus/main_menu.json", EM, Factory);    

    EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(musicMenu->id, 0));
    
}

void MainMenuScene::update(float deltaTime)
{
    //WARNING ALE
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
    ESystem.update(EVM, EM);
    Ssystem.update_menus(EM, gameManager.getSoundEngine());
}

void MainMenuScene::update_sim(float simDeltaTime)
{
    
}

void MainMenuScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void MainMenuScene::cleanup()
{
    gameManager.getRenderManager()->quitVideo();
    EM.resetEntityManager();
    
}