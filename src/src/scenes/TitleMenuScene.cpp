#include "TitleMenuScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

void TitleMenuScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
    std::vector<dataSprite> dataMenu {};
    std::array<std::string, 4> buttonData {};
    JSONManager jsonManager;
    jsonManager.loadJSON(path);
    jsonManager.loadMenuItems(dataMenu);
    jsonManager.loadJSON("assets/JSON/renderManager/sprites.json");

    JSONManager jsonButtonManager;
    jsonButtonManager.loadJSON("assets/JSON/renderManager/button.json");
    
    int i = 0;
    for(const auto& item : dataMenu){
        if(item.button==1){       
            int idSprite = jsonButtonManager.getButtonMapping(item.id);
            jsonManager.getInfo2D(idSprite, buttonData);
            EF.createButton(EM, gameManager.getSoundEngine(), item.id, idSprite, item.posX, item.posY, std::stof(buttonData[2]), std::stof(buttonData[3]), item.alpha);
        }else{
            sprites[i] = &EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
            i++;
        }
    }
    
    musicMenu = &EF.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/pagina-inical-music");
    gameManager.idMusicMenu = musicMenu->id;
}

void TitleMenuScene::init()
{    
    loadMenu("assets/JSON/menus/title_menu.json", EM, Factory);    

    initTimer = false;
    timer = 0.0f;
}


void TitleMenuScene::update(float deltaTime)
{
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
    ESystem.update(EVM, EM);
    Ssystem.update_menus(EM, gameManager.getSoundEngine());

    if(!initTimer){
        timer = gameManager.getRenderManager()->getWindowTime();
        initTimer = true;
    }

    if((gameManager.getRenderManager()->getWindowTime()-timer) < timeToLoad)
    {
        float time = (gameManager.getRenderManager()->getWindowTime()-timer);
        float alpha = static_cast<int>(time * (255.0f / timeToLoad));
        sprites[0]->getParent().getComponent<Render2dComponent>(sprites[0]->getComponentKey<Render2dComponent>().value()).color.a = alpha;
    }

    if((gameManager.getRenderManager()->getWindowTime()-timer) > timeToLoad)
    {
        loadMenu("assets/JSON/menus/pre_charge_menus.json", EM, Factory);    
        // CARGO LOS 3D DE MENUS
        JSONManager json3dIds;
        json3dIds.loadJSON("assets/JSON/menus/pre_charge_menus3d.json");
        std::vector<int> loads3d = json3dIds.getUnlockCars();
        for(auto &i:loads3d){
            myMesh mesh;
            mesh.position = {0.0f, 0.0f, 0.0f};
            mesh.rotation = {0.0f, 0.0f, 0.0f, 1.0f};
            mesh.id = i;
            gameManager.getRenderManager()->loadingAssets(mesh);
        }


        gameManager.changeScene<MainMenuScene>();
    }
}

void TitleMenuScene::update_sim(float simDeltaTime)
{
    // Actualizar la escena
}

void TitleMenuScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void TitleMenuScene::cleanup()
{
    EM.resetEntityManager();
}

