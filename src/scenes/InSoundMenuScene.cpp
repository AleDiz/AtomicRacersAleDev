#include "InSoundMenuScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

void InSoundMenuScene::updateVolumeSprites(int startIdx, int endIdx, int volume) {
    int spritesToActivate = volume / 10; // Calculate how many sprites to activate
    for (int i = startIdx; i <= endIdx; i++) {
        if (sprites[i] != nullptr) {
            if (i - startIdx < spritesToActivate) {
                sprites[i]->getParent().getComponent<Render2dComponent>(sprites[i]->getComponentKey<Render2dComponent>().value()).color.a = 255;
            } else {
                sprites[i]->getParent().getComponent<Render2dComponent>(sprites[i]->getComponentKey<Render2dComponent>().value()).color.a = 0;
            }
        }
    }
}

void InSoundMenuScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
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
    int volumeMaster = jsonManagerOpciones.getInt("volumeMaster");
    int volumeMusic = jsonManagerOpciones.getInt("volumeMusic");
    int volumeSfx = jsonManagerOpciones.getInt("volumeSfx");


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
    
    auto updateVolumeSprites = [&](int startIdx, int endIdx, int volume) {
        int spritesToActivate = volume / 10; // Calculate how many sprites to activate
        for (int i = startIdx; i <= endIdx; i++) {
            if (sprites[i] != nullptr) {
                if (i - startIdx < spritesToActivate) {
                    sprites[i]->getParent().getComponent<Render2dComponent>(sprites[i]->getComponentKey<Render2dComponent>().value()).color.a = 255;
                } else {
                    sprites[i]->getParent().getComponent<Render2dComponent>(sprites[i]->getComponentKey<Render2dComponent>().value()).color.a = 0;
                }
            }
        }
    };

    updateVolumeSprites(6, 15, volumeMaster);
    updateVolumeSprites(16, 25, volumeMusic);
    updateVolumeSprites(26, 35, volumeSfx);
    
    musicMenu = &EF.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/pagina-inical-music");
    gameManager.idMusicMenu = musicMenu->id;

}

void InSoundMenuScene::init()
{
    loadMenu("assets/JSON/menus/in_audio_menu.json", EM, Factory);    
}

void InSoundMenuScene::update(float deltaTime)
{
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    
    Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
    ESystem.update(EVM, EM);
    Ssystem.update_menus(EM, gameManager.getSoundEngine());

    JSONManager jsonManagerOpciones;
    jsonManagerOpciones.loadJSON("assets/JSON/player/state.json");
    int volumeMaster = jsonManagerOpciones.getInt("volumeMaster");
    int volumeMusic = jsonManagerOpciones.getInt("volumeMusic");
    int volumeSfx = jsonManagerOpciones.getInt("volumeSfx");

    updateVolumeSprites(6, 15, volumeMaster);
    updateVolumeSprites(16, 25, volumeMusic);
    updateVolumeSprites(26, 35, volumeSfx);
}

void InSoundMenuScene::update_sim(float simDeltaTime)
{
    // Actualizar la escena
}

void InSoundMenuScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void InSoundMenuScene::cleanup()
{
    EM.resetEntityManager();
}
