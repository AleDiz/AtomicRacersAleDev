#include "InVideoMenuScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

void InVideoMenuScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
    std::vector<dataSprite> dataMenu {};
    std::array<std::string, 4> buttonData {};
    JSONManager jsonManager;
    jsonManager.loadJSON(path);
    jsonManager.loadMenuItems(dataMenu);
    jsonManager.loadJSON("assets/JSON/renderManager/sprites.json");

    JSONManager jsonButtonManager;
    jsonButtonManager.loadJSON("assets/JSON/renderManager/button.json");

    JSONManager jsonManagerOpciones;
    jsonManagerOpciones.loadJSON("assets/JSON/player/state.json");
    int resolucion = jsonManagerOpciones.getInt("resolucion");
    int modoPantalla = jsonManagerOpciones.getInt("modoPantalla");
    int motor = jsonManagerOpciones.getInt("motor");

    auto scene = gameManager.getLastScene();

    int i = 0;
    for(const auto& item : dataMenu){
        int alpha = 255;
        if(item.button==1){ 
            switch (item.id)
            {
                case 13:
                {
                    if(resolucion == 1)
                    alpha = 0;
                    break;
                }
                case 14:
                {
                    if(resolucion == 0)
                    alpha = 0;
                    break;
                }
                case 15:
                {
                    if(modoPantalla == 1)
                    alpha = 0;
                    break;
                }
                case 16:
                {
                    if(modoPantalla == 0)
                    alpha = 0;
                    break;
                }
                case 17:
                {
                    if(motor == 1)
                    alpha = 0;
                    break;
                }
                case 18:
                {
                    if(motor == 0)
                    alpha = 0;
                    break;
                }
            }
            
            int idSprite = jsonButtonManager.getButtonMapping(item.id);
            jsonManager.getInfo2D(idSprite, buttonData);
            buttons[i] = &EF.createButton(EM, gameManager.getSoundEngine(), item.id, idSprite, item.posX, item.posY, std::stof(buttonData[2]), std::stof(buttonData[3]), alpha);
            i++;
        }else{
            EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
        }
    }
    
    musicMenu = &EF.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/pagina-inical-music");
    gameManager.idMusicMenu = musicMenu->id;

}

void InVideoMenuScene::init()
{
    loadMenu("assets/JSON/menus/in_video_menu.json", EM, Factory);    
}

void InVideoMenuScene::update(float deltaTime)
{
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
    ESystem.update(EVM, EM);
    Ssystem.update_menus(EM, gameManager.getSoundEngine());

    JSONManager jsonManagerOpciones;
    jsonManagerOpciones.loadJSON("assets/JSON/player/state.json");
    int resolucion = jsonManagerOpciones.getInt("resolucion");
    int modoPantalla = jsonManagerOpciones.getInt("modoPantalla");
    int motor = jsonManagerOpciones.getInt("motor");

    if(resolucion == 1)
    {
        buttons[0]->getParent().getComponent<Render2dComponent>(buttons[0]->getComponentKey<Render2dComponent>().value()).color.a = 0;
        buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 255;
    }
    else
    {
        buttons[0]->getParent().getComponent<Render2dComponent>(buttons[0]->getComponentKey<Render2dComponent>().value()).color.a = 255;
        buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 0;
    }

    if(modoPantalla == 1)
    {
        buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a = 0;
        buttons[3]->getParent().getComponent<Render2dComponent>(buttons[3]->getComponentKey<Render2dComponent>().value()).color.a = 255;
    }
    else
    {
        buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a = 255;
        buttons[3]->getParent().getComponent<Render2dComponent>(buttons[3]->getComponentKey<Render2dComponent>().value()).color.a = 0;
    }

    if(motor == 1)
    {
        buttons[4]->getParent().getComponent<Render2dComponent>(buttons[4]->getComponentKey<Render2dComponent>().value()).color.a = 0;
        buttons[5]->getParent().getComponent<Render2dComponent>(buttons[5]->getComponentKey<Render2dComponent>().value()).color.a = 255;
    }
    else
    {
        buttons[4]->getParent().getComponent<Render2dComponent>(buttons[4]->getComponentKey<Render2dComponent>().value()).color.a = 255;
        buttons[5]->getParent().getComponent<Render2dComponent>(buttons[5]->getComponentKey<Render2dComponent>().value()).color.a = 0;
    }
}

void InVideoMenuScene::update_sim(float simDeltaTime)
{
    // Actualizar la escena
}

void InVideoMenuScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void InVideoMenuScene::cleanup()
{
    EM.resetEntityManager();
}
