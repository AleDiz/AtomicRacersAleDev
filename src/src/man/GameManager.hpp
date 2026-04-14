#pragma once

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <tuple>

#include "../util/JsonManager.hpp"

#include "../commons/SceneCommons.h"
#include "RenderManager.hpp"

class ISound;
enum class RenderType;

class Scene;

class GameManager 
{
public:
    // Constructor
    GameManager(int WW, int WH, const char* windowName, int fps ,RenderType renderType);

    // Método para cambiar de escena
    template <typename SceneT> 
    void changeScene()
    {
        isInitialized = false;
        lastScene = currentScene;
        futureScene = &std::get<SceneT>(scenes);
        if (lastScene == &std::get<InGameOptionsScene>(scenes) && std::is_same_v<SceneT, MainMenuScene>)
        {
            resetGameScene = true;
            eraseGameScene = true;
        }
        if (lastScene == &std::get<GameScene>(scenes) && std::is_same_v<SceneT, InGameOptionsScene>)
        {
            cleanupScene = false;
        }
    }

    void checkChangeScene()
    {
        if (futureScene != nullptr)
        {
            if (futureScene != currentScene)
            {
                if(resetGameScene && eraseGameScene)
                {
                    std::get<GameScene>(scenes).cleanup();
                    eraseGameScene = false;
                }
                
                if(cleanupScene)
                {
                    currentScene->cleanup();
                }
                currentScene = futureScene;
                futureScene = nullptr;
                isInitialized = false;
                cleanupScene = true;
            }
        }
    }                                            

    // Con esto podremos saber si el la escena de juego debe hacer init la primera vez que entra
    void setResetGameScene(bool reset);               
    bool getResetGameScene () { return resetGameScene; }

    // Métodos para obtener la escena actual
    Scene * getCurrentScene() { return currentScene; }
    Scene * getLastScene() { return lastScene; }
    Scene * getScene() { return currentScene; }
    void setLastScene(Scene* scene) { lastScene = scene; }
    //Scene* getScene(Scene* scene){ return &std::get<scene>(scenes)}    // WARNINGANGEL: Mirar como se hace esto

    // Método par obtener el config JSON
    JSONManager& getJSONManager(){return jsonManager; }

    // Metodos para los sonidos
    void releaseSound(){ soundEngine->shutdown(); } 
    void initSoundEngine();
    ISound* getSoundEngine(){ return soundEngine.get(); }

    RenderManager* getRenderManager(){return &RM; }

    // Metodo para captar input
    myInput updateLastInput();
    myInput getLastInput(); 
    void    setLastInput(int r);    

    float getInterpolationFactor();   

    // Método para ejecutar el juego
    void run();

    // Método para finalizar el juego
    void finishGame(){ runingGame = false; }

    int idMusicMenu{};
    E* musicMenu;

private:
    // Bool para saber si la escena ya se ha inicializado
    bool isInitialized {false};      
    
    // Bool para saber si la escena de juego debe hacer init la primera vez que entra
    bool resetGameScene{true};   
    bool eraseGameScene{false}; 
    bool cleanupScene{true};
    bool continueUpdating{false};

    // Código para saber si el juego debe cerrarse
    bool runingGame{true};

    // Config JSON
    JSONManager jsonManager;

    // FmodSoundEngine soundEngine;
    std::unique_ptr<ISound> soundEngine;

    // Referencia al motor gráfico
    RenderManager RM;                     

    // Último input
    myInput lastInput{};

    int frameCounter{0};

    float interpolationFactor {0.0f};

    // Escenas
    std::tuple< CarCollectionScene, 
                ChargeScreenScene, 
                EndGameScene, 
                GameScene,
                DebugGameScene, 
                InGameOptionsScene, 
                MainMenuScene, 
                OutGameOptionsScene, 
                PreGameScene,
                PreOnlineGameScene, 
                ShopScene, 
                InSoundMenuScene,
                OutSoundMenuScene, 
                TitleMenuScene, 
                InVideoMenuScene,
                OutVideoMenuScene,
                InControlesScene,
                OutControlesScene,
                OnlineGameScene,
                GachaScene>  
                scenes{*this, *this, *this , *this , *this , *this, *this, *this, *this, *this, *this, *this, *this, *this, *this, *this, *this, *this, *this, *this};
                
    // Escena actual y última escena
    Scene * futureScene{};
    Scene * currentScene{};
    Scene * lastScene{};
};

#endif 
