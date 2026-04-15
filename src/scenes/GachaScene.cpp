#include "GachaScene.hpp"

#include "../man/GameManager.hpp"
#include "../util/JsonManager.hpp"

#include <cstdlib>

using namespace std;

void GachaScene::comprobarTirada()
{
    if(buttons[1])
    {
        if((buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a == 0) && tirada)
        {
            tirada = false;
            tirar();
        }
    }
}

void GachaScene::calcularMonedas()
{

    JSONManager jsonManagerStatePlayer;
    jsonManagerStatePlayer.loadJSON("assets/JSON/player/state.json");
    int monedas = jsonManagerStatePlayer.getInt("monedas");

    int unidades = monedas % 10;
    int decenas = (monedas / 10) % 10;
    int centenas = (monedas / 100) % 10;

    // Reset alpha for all numbers
    for (auto& number : numbers) {
        if (number) {
            number->getParent().getComponent<Render2dComponent>(number->getComponentKey<Render2dComponent>().value()).color.a = 0;
        }
    }

    // Set alpha for the corresponding numbers
    if (numbers[unidades]) 
    {
        numbers[unidades]->getParent().getComponent<Render2dComponent>(numbers[unidades]->getComponentKey<Render2dComponent>().value()).color.a = 255;
    }
    if (numbers[10 + decenas]) 
    {
        numbers[10 + decenas]->getParent().getComponent<Render2dComponent>(numbers[10 +decenas]->getComponentKey<Render2dComponent>().value()).color.a = 255;
    }
    if (numbers[20 + centenas]) 
    {
        numbers[20 + centenas]->getParent().getComponent<Render2dComponent>(numbers[20 +centenas]->getComponentKey<Render2dComponent>().value()).color.a = 255;
    }
}

void GachaScene::tirar()
{
    int monedas {0};
    JSONManager jsonManagerStatePlayer;
    jsonManagerStatePlayer.loadJSON("assets/JSON/player/state.json");
    jsonManagerStatePlayer.getIntWithKey("monedas", monedas);

    if(monedas < 100)
    {
        buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 255;
        tirada = true;
        return;
    }

    monedas -= 100;
    jsonManagerStatePlayer.setInt("monedas",monedas);    
    jsonManagerStatePlayer.saveJSON("assets/JSON/player/state.json");
    
    int valor {};
    Rareza rareza {};
    vector<int> ids {};

    valor = rand() % MAX_RAND_RARERITY;

    if(valor <= 6)  rareza = Rareza::COMUN;
    else if(valor <= 8) rareza = Rareza::RARO;
    else rareza = Rareza::LEGENDARIO;

    JSONManager jsonManagerBD_cars;
    jsonManagerBD_cars.loadJSON("assets/JSON/gacha/BD_cars.json");
    jsonManagerBD_cars.getInfoRarerity(static_cast<int>(rareza), ids);

    // Nunca va a pasar porque permitimos que salgan repetidos. Si sale repe se suman monedas en la logica de addCarToCollection
    if(ids.empty())
    {
        return;
    }
    int id = ids[rand() % ids.size()];
    estado = GachaState::TIRADA;
    idObtenido = id;
    jsonManagerStatePlayer.loadJSON("assets/JSON/player/state.json");

}

void GachaScene::loadMenu(std::string path, EManager& EM, EntityFactory& EF){
    std::vector<dataSprite> dataMenu {};
    std::array<std::string, 4> buttonData {};
    JSONManager jsonManager;
    jsonManager.loadJSON(path);
    jsonManager.loadMenuItems(dataMenu);
    jsonManager.loadJSON("assets/JSON/renderManager/sprites.json");

    JSONManager jsonButtonManager;
    jsonButtonManager.loadJSON("assets/JSON/renderManager/button.json");
    
    int nButton = 0;
    int nNumber = 0;
    int nSprite = 0;
    for(const auto& item : dataMenu)
    {
        if(item.button==1)
        {
            int idSprite = jsonButtonManager.getButtonMapping(item.id);
            jsonManager.getInfo2D(idSprite, buttonData);
            buttons[nButton] = &EF.createButton(EM, gameManager.getSoundEngine(), item.id, idSprite, item.posX, item.posY, std::stof(buttonData[2]), std::stof(buttonData[3]), item.alpha);
            nButton++;
        }
        else
        {
            jsonManager.getInfo2D(item.id, buttonData);
            if(item.id == 86 || item.id == 87 || item.id == 88 || item.id == 89 || item.id == 90 || item.id == 91
            || item.id == 92 || item.id == 93 || item.id == 94 || item.id == 95)
            {
                numbers[nNumber] = &EF.createButton(EM, gameManager.getSoundEngine(), item.id, item.id, item.posX, item.posY, std::stof(buttonData[2]), std::stof(buttonData[3]), item.alpha);
                nNumber++;
            }
            else
            {
                if (item.id > 1000)
                {
                    EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
                }else{
                    sprites[nSprite] = &EF.createSprite(EM, item.id, item.posX, item.posY, item.alpha);
                    nSprite++;
                }
                
            }
        }
    }

    calcularMonedas();
    
    musicMenu = &EF.createEntityMusic(EM, gameManager.getSoundEngine(), "event:/Musica/pagina-inical-music");
    gameManager.idMusicMenu = musicMenu->id;

}

void GachaScene::init()
{    
    loadMenu("assets/JSON/menus/gacha.json", EM, Factory);    

    initTimer = false;
    timer = 0.0f;
}


void GachaScene::update(float deltaTime)
{
    myVector2 mousePos = gameManager.getRenderManager()->getMousePos();
    myWindow window = gameManager.getRenderManager()->getWindow();
    bool clicked = gameManager.getRenderManager()->getMouseClick();
    switch (estado)
    {
    case GachaState::PRE:
        Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
        ESystem.update(EVM, EM);
        Ssystem.update_menus(EM, gameManager.getSoundEngine());

        calcularMonedas();
        comprobarTirada();
        break;
    case GachaState::TIRADA:
        if(!initTimer){
            timer = gameManager.getRenderManager()->getWindowTime();
            initTimer = true;
            // for ( auto& i : sprites){
            //     i->getParent().getComponent<Render2dComponent>(i->getComponentKey<Render2dComponent>().value()).color.a = 0;
            // }
            // for ( auto& i : buttons){
            //     i->getParent().getComponent<Render2dComponent>(i->getComponentKey<Render2dComponent>().value()).color.a = 0;
            // }
            // for ( auto& i : numbers){
            //     i->getParent().getComponent<Render2dComponent>(i->getComponentKey<Render2dComponent>().value()).color.a = 0;
            // }
            sprites[0]->getParent().getComponent<Render2dComponent>(sprites[0]->getComponentKey<Render2dComponent>().value()).color.a = 0;
            gameManager.getRenderManager()->setVideoPaused(false);
            calcularMonedas();

        }

        if( gameManager.getRenderManager()->getWindowTime() - timer > 5.1f){
            JSONManager jsonManagerStatePlayer;
            jsonManagerStatePlayer.loadJSON("assets/JSON/player/state.json");
            if(jsonManagerStatePlayer.addCarToCollection(idObtenido))
            {
                sprites[4]->getParent().getComponent<Render2dComponent>(sprites[4]->getComponentKey<Render2dComponent>().value()).color.a = 255;
        
                muestra = &Factory.createMuestra(EM, idObtenido);
                camera = &Factory.createEntityFixedCamera(EM, {0.0f, 1.0f, -5.0f}, {0.0f,0.0f,0.0f});
        
                buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a = 255;
                buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 0;
                recompensa = true;
            }
            else
            {
                sprites[3]->getParent().getComponent<Render2dComponent>(sprites[3]->getComponentKey<Render2dComponent>().value()).color.a = 255;
        
                muestra = &Factory.createMuestra(EM, idObtenido);
                camera = &Factory.createEntityFixedCamera(EM, {0.0f, 1.0f, -5.0f}, {0.0f,0.0f,0.0f});
        
                buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a = 255;
                buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 0;
                repetido = true;
        
            }
            jsonManagerStatePlayer.saveJSON("assets/JSON/player/state.json");
            sprites[0]->getParent().getComponent<Render2dComponent>(sprites[0]->getComponentKey<Render2dComponent>().value()).color.a = 255;
            estado = GachaState::POS;
            initTimer = false;
        }
        break;
    case GachaState::POS:
        Bsystem.update(EM,gameManager, EVM, window.WW, window.WH, mousePos.x, mousePos.y, clicked);
        ESystem.update(EVM, EM);
        Ssystem.update_menus(EM, gameManager.getSoundEngine());
        if((buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a == 0) && recompensa)
        {
            muestra->state = false;
            camera->state = false;
            EM.comprobarState();
            
            buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a = 0;
            sprites[4]->getParent().getComponent<Render2dComponent>(sprites[4]->getComponentKey<Render2dComponent>().value()).color.a = 0;
            buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 255;
            tirada = true;
            recompensa = false;
            estado = GachaState::PRE;
            gameManager.getRenderManager()->quitVideo();

        }

        if((buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a == 0) && repetido)
        {
            muestra->state = false;
            camera->state = false;
            EM.comprobarState();
            
            buttons[2]->getParent().getComponent<Render2dComponent>(buttons[2]->getComponentKey<Render2dComponent>().value()).color.a = 0;
            sprites[3]->getParent().getComponent<Render2dComponent>(sprites[3]->getComponentKey<Render2dComponent>().value()).color.a = 0;
            buttons[1]->getParent().getComponent<Render2dComponent>(buttons[1]->getComponentKey<Render2dComponent>().value()).color.a = 255;
            tirada = true;
            repetido = false;
            estado = GachaState::PRE;
            gameManager.getRenderManager()->quitVideo();

        }
        break;
    }

}

void GachaScene::update_sim(float simDeltaTime)
{
    // Actualizar la escena
}

void GachaScene::render()
{
    Rsystem.update(EM, gameManager.getRenderManager());
}

void GachaScene::cleanup()
{
    gameManager.getRenderManager()->quitVideo();
    EM.resetEntityManager();
}