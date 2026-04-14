#include "GameManager.hpp"

#include "../util/RenderTypes.hpp"
#include "../engine/FmodSoundEngine.cpp"

#include "../scenes/Scene.hpp"

#include <GLFW/glfw3.h>

// Constructor
GameManager::GameManager(int WW, int WH, const char *windowName, int fps,RenderType renderType)
    : RM(WW,WH,windowName, fps,renderType), currentScene {&std::get<TitleMenuScene>(scenes)} // Inicialización del motor gráfico
{
    initSoundEngine();

    // Obtener valores de configuración - WARNINGJAVIER: ESTO DE LOS SONIDOS COMO VA??????
    // getJSONManager().loadConfig("assets/configOptions.json");
    // 
    // int track1 = getJSONManager().getInt("Track1");
    // int track2 = getJSONManager().getInt("Track2");
    // [[maybe_unused]] int track3 = getJSONManager().getInt("Track3");

    // getSoundEngine()->setGroupVolume("vca:/MUSIC", ((static_cast<float>(track1) / 100.0f) * (static_cast<float>(track1) / 100.0f)));
    // getSoundEngine()->setGroupVolume("vca:/SFX", ((static_cast<float>(track2) / 100.0f) * (static_cast<float>(track2) / 100.0f)));    

    JSONManager jsonPlayer;
    jsonPlayer.loadJSON("assets/JSON/player/state.json");
    float volumeMusic, volumeSFX, volumeMaster {};
    jsonPlayer.getFloatWithKey("volumeMusic", volumeMusic);
    jsonPlayer.getFloatWithKey("volumeSfx", volumeSFX);
    jsonPlayer.getFloatWithKey("volumeMaster", volumeMaster);

    volumeMusic = volumeMusic / 100.0f;
    volumeSFX = volumeSFX / 100.0f;
    volumeMaster = volumeMaster / 100.0f;
    
    soundEngine->setGroupVolume("vca:/MUSIC", (volumeMusic * volumeMusic));
    soundEngine->setGroupVolume("vca:/SFX", (volumeSFX * volumeSFX));
    soundEngine->setGroupVolume("vca:/MASTER", (volumeMaster * volumeMaster));
    std::vector<int> unlockedCars = jsonPlayer.getUnlockCars();
}

void GameManager::setResetGameScene(bool reset){
    resetGameScene = reset;
}

void GameManager::run()
{
    using Clock = std::chrono::high_resolution_clock;
    auto previousTime = Clock::now();

    float simAccumulation = 0.0f;
    float simDeltaTime = 1.0f / 15.0f;  // Simulación a 15 FPS

    while (runingGame)
    {
        auto currentTime = Clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count(); // Dinámico
        previousTime = currentTime;

        runingGame = !RM.windowShouldClose();
        simAccumulation += deltaTime;

        if (!isInitialized)
        {
            currentScene->init();
            if (getCurrentScene() == &std::get<DebugGameScene>(scenes)) {
                RM.setDebugRender(true);
                RM.setDebugWorld(std::get<DebugGameScene>(scenes).getWorld());
            }
            isInitialized = true;
        }

        int simSteps = 0;
        while (simAccumulation >= simDeltaTime) {
            simAccumulation -= simDeltaTime;
            currentScene->update_sim(simDeltaTime);
            simSteps++;
        }

        updateLastInput();

        interpolationFactor = simAccumulation / simDeltaTime;

        currentScene->update(deltaTime);
        currentScene->render();

        soundEngine->update();
        checkChangeScene();

        // Control de framerate opcional
        float targetFrameTime = 1.0f / 60.0f;
        auto frameEnd = Clock::now();
        float frameDuration = std::chrono::duration<float>(frameEnd - currentTime).count();

        if (frameDuration < targetFrameTime) {
            std::this_thread::sleep_for(
                std::chrono::duration<float>(targetFrameTime - frameDuration)
            );
        }
    }
}

void GameManager::initSoundEngine(){
    soundEngine = std::make_unique<FmodSoundEngine>();
    soundEngine->init("");
}

//Metodo para captar input

myInput GameManager::updateLastInput(){
    //w,a,s,d,espacio,e,shift,enter
    auto actualInput = RM.updateInput();
    
    lastInput = actualInput;
    return lastInput;
}

myInput GameManager::getLastInput(){
    return lastInput;
}

void GameManager::setLastInput(int r){
    lastInput.inputMask = r;
}   


float GameManager::getInterpolationFactor(){
    return interpolationFactor;
}