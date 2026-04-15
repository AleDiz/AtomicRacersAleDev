#include "ButtonSystem.hpp"

#include "../man/GameManager.hpp"
#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"

#include "../components/ButtonComponent.hpp"

int idActHover = -1;
int idLastHover = -1;

void ButtonSystem::update(EManager& EM, GameManager& GM, EventManager& EVM, int WW, int WH, int MX, int MY, bool clicked){
    EM.forAllCondition<void(*)(E&, GameManager&, EventManager&, int, int, int, int, bool), ButtonComponent>(update_one_entity_button,GM, EVM, WW, WH, MX, MY, clicked);
};

void ButtonSystem::update_one_entity_button(E& e, GameManager& GM, EventManager& EVM, int WW, int WH, int MX, int MY, bool clicked)
{
    auto& b = e.getParent().getComponent<ButtonComponent>(e.getComponentKey<ButtonComponent>().value());
    auto& r = e.getParent().getComponent<Render2dComponent>(e.getComponentKey<Render2dComponent>().value());
    auto& s = e.getParent().getComponent<SoundComponent>(e.getComponentKey<SoundComponent>().value());


    int posXAct = b.posX;
    int posYAct = b.posY;
    int sizeXAct = b.sizeX;
    int sizeYAct = b.sizeY;

    b.hover = false;
    

    if(WW != 1920 || WH != 1080){
        float scale = static_cast<float>(WW) / 1920.0f;
        posXAct *= scale;
        posYAct *= scale;
        sizeXAct *= scale;
        sizeYAct *= scale;
    }
    //TODAVIA NO CONTEMPLADO EL 4:3 SIEMPRE SE CALCULA PARA 16:9

    JSONManager& JM = GM.getJSONManager();
    JM.loadJSON("assets/JSON/playerConfig.json");
    int coche = JM.getInt("PlayerCar");

    JSONManager JMOpciones;
    JMOpciones.loadJSON("assets/JSON/player/state.json");

    if(coche == b.id)
    {
        b.hover = true;
    }

    if(MX < posXAct + sizeXAct && MX > posXAct)
    {
        if(MY < posYAct + sizeYAct && MY > posYAct)
        {

            if(r.color.a != 0)
            {
                idLastHover = idActHover;
                idActHover = b.id;
            }

            if(idActHover != idLastHover)
            {
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 1));
            }
            
            if(clicked && r.color.a != 0)
            {
                EVM.addEvent(EventTypes::PLAYSOUND, std::make_unique<PlaySoundData>(e.id, 0));
                switch (b.id)
                {
                case 1:
                    {
                        GM.changeScene<PreGameScene>();
                        break;
                    }
                case 2:
                    {
                        GM.changeScene<PreOnlineGameScene>();
                        break;
                    }
                case 3:
                    {
                        GM.changeScene<CarCollectionScene>();
                        break;
                    }
                case 4:
                    {
                        GM.changeScene<GachaScene>();
                        break;
                    }
                case 5:
                    {
                        GM.changeScene<OutGameOptionsScene>();
                        break;
                    }
                case 6:
                    {
                        GM.finishGame();
                        break;
                    }
                case 7:
                    {
                        JM.setInt("PlayerCar", 501);
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 8:
                    {
                        JM.setInt("PlayerCar", 511);
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 9:
                    {
                        JM.setInt("PlayerCar", 521);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 10:
                    {
                        GM.changeScene<GameScene>();
                        break;
                    }
                case 11:
                    {
                        GM.changeScene<OnlineGameScene>();
                        break;
                    }
                case 12:
                    {
                        GM.changeScene<MainMenuScene>();
                        break;
                    }
                case 13:
                    {
                        JMOpciones.setInt("resolucion", 1);
                        JMOpciones.saveJSON("assets/JSON/player/state.json");
                        GM.getRenderManager()->configure(1920, 1080, 60);
                        break;
                    }
                case 14:
                    {
                        if(JMOpciones.getInt("modoPantalla") == 1)
                        {
                            JMOpciones.setInt("modoPantalla", 0);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            GM.getRenderManager()->setWindow(0);
                        }
                        JMOpciones.setInt("resolucion", 0);
                        JMOpciones.saveJSON("assets/JSON/player/state.json");
                        GM.getRenderManager()->configure(1280, 720, 60);
                        break;
                    }
                case 15:
                    {
                        if(JMOpciones.getInt("resolucion") == 0)
                        {
                            JMOpciones.setInt("resolucion", 1);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            GM.getRenderManager()->configure(1920, 1080, 60);
                        }
                        JMOpciones.setInt("modoPantalla", 1);
                        JMOpciones.saveJSON("assets/JSON/player/state.json");
                        GM.getRenderManager()->setWindow(1);
                        break;
                    }
                case 16:
                    {
                        JMOpciones.setInt("modoPantalla", 0);
                        JMOpciones.saveJSON("assets/JSON/player/state.json");
                        GM.getRenderManager()->setWindow(0);
                        break;
                    }
                case 17:
                    {
                        GM.getRenderManager()->changerRenderer();
                        JMOpciones.setInt("motor", 1);
                        JMOpciones.saveJSON("assets/JSON/player/state.json");
                        break;
                    }
                case 18:
                    {
                        GM.getRenderManager()->changerRenderer();
                        JMOpciones.setInt("motor", 0);
                        JMOpciones.saveJSON("assets/JSON/player/state.json");
                        break;
                    }
                case 19:
                    {
                        GM.changeScene<OutSoundMenuScene>(); // ESCENA AUDIO OUT
                        break;
                    }
                case 20:
                    {
                        GM.changeScene<InSoundMenuScene>(); // ESCEBA AUDIO IN
                        break;
                    }
                case 21:
                    {
                        GM.changeScene<OutVideoMenuScene>(); // ESCENA VIDEO OUT
                        break;
                    }
                case 22:
                    {
                        GM.changeScene<InVideoMenuScene>(); // ESCENA VIDEO IN
                        break;
                    }
                case 23:
                    {
                        GM.changeScene<OutGameOptionsScene>();
                        break;
                    }
                case 24:
                    {
                        GM.changeScene<InGameOptionsScene>();
                        break;
                    }
                case 25:
                    {
                        GM.changeScene<MainMenuScene>(); // ESCENA MENU PRINCIPAL
                        break;
                    }
                case 26:
                    {
                        GM.changeScene<GameScene>(); // ESCENA JUEGO - BOTON VOLVER
                        break;
                    }
                case 27:
                    {
                        float volume;
                        JMOpciones.getFloatWithKey("volumeMaster", volume);
                        if (volume > 0)
                        {
                            volume -= 10;
                            JMOpciones.setIntWithKey("volumeMaster", volume);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            volume = volume / 100.0f;
                            s.getParent()->setGroupVolume("vca:/MASTER", (volume * volume));
                        }
                        break;
                    }
                case 28:
                    {
                        float volume;
                        JMOpciones.getFloatWithKey("volumeMusic", volume);
                        if (volume > 0)
                        {
                            volume -= 10;
                            JMOpciones.setIntWithKey("volumeMusic", volume);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            volume = volume / 100.0f;
                            s.getParent()->setGroupVolume("vca:/MUSIC", (volume * volume));
                        }
                        break;
                    }
                case 29:
                    {
                        float volume;
                        JMOpciones.getFloatWithKey("volumeSfx", volume);
                        if (volume > 0)
                        {
                            volume -= 10;
                            JMOpciones.setIntWithKey("volumeSfx", volume);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            volume = volume / 100.0f;
                            s.getParent()->setGroupVolume("vca:/SFX", (volume * volume));
                        }
                        break;
                    }
                case 30:
                    {
                        float volume;
                        JMOpciones.getFloatWithKey("volumeMaster", volume);
                        if (volume < 100)
                        {
                            volume += 10;
                            JMOpciones.setIntWithKey("volumeMaster", volume);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            volume = volume / 100.0f;
                            s.getParent()->setGroupVolume("vca:/MASTER", (volume * volume));
                        }
                        break;
                    }
                case 31:
                    {
                        float volume;
                        JMOpciones.getFloatWithKey("volumeMusic", volume);
                        if (volume < 100)
                        {
                            volume += 10;
                            JMOpciones.setIntWithKey("volumeMusic", volume);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            volume = volume / 100.0f;
                            s.getParent()->setGroupVolume("vca:/MUSIC", (volume * volume));

                        }
                        break;
                    }
                case 32:
                    {
                        float volume;
                        JMOpciones.getFloatWithKey("volumeSfx", volume);
                        if (volume < 100)
                        {
                            volume += 10;
                            JMOpciones.setIntWithKey("volumeSfx", volume);
                            JMOpciones.saveJSON("assets/JSON/player/state.json");
                            volume = volume / 100.0f;
                            s.getParent()->setGroupVolume("vca:/SFX", (volume * volume));
                        }
                        break;
                    }
                case 33:
                    {
                        GM.changeScene<OutControlesScene>();
                        break;
                    }
                case 34:
                    {
                        GM.changeScene<InControlesScene>();
                        break;
                    }
                case 35:
                    {
                        // ESTO ES EL BOTON DE TIRAR
                        r.color.a = 0;
                        break;
                    }
                case 36:
                    {
                        // ESTO ES EL BOTON DE ACEPTAR
                        r.color.a = 0;
                        break;
                    }
                case 501:
                    {
                        JM.setInt("PlayerCar", 501);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 502:
                    {
                        JM.setInt("PlayerCar", 502);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }  
                case 503:
                    {
                        JM.setInt("PlayerCar", 503);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 504:
                    {
                        JM.setInt("PlayerCar", 504);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 505:
                    {
                        JM.setInt("PlayerCar", 505);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 511:
                    {
                        JM.setInt("PlayerCar", 511);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 512:
                    {
                        JM.setInt("PlayerCar", 512);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }  
                case 513:
                    {
                        JM.setInt("PlayerCar", 513);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 514:
                    {
                        JM.setInt("PlayerCar", 514);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 515:
                    {
                        JM.setInt("PlayerCar", 515);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 521:
                    {
                        JM.setInt("PlayerCar", 521);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 522:
                    {
                        JM.setInt("PlayerCar", 522);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }  
                case 523:
                    {
                        JM.setInt("PlayerCar", 523);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 524:
                    {
                        JM.setInt("PlayerCar", 524);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                case 525:
                    {
                        JM.setInt("PlayerCar", 525);                        
                        JM.saveJSON("assets/JSON/playerConfig.json");
                        break;
                    }
                }
            }
            b.hover = true;
        }
    }


    //FALTA LA ACTUALIZACION DE LA ACCION DEL BOTON
};
