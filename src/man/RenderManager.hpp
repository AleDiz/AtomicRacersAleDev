#pragma once

#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <vector>
#include <memory>
#include "../util/RenderTypes.hpp"
#include "../util/RenderUtils.hpp"
#include "../engine/GraphicsAPI.hpp"

//SOLO PARA EL DEBUGGER BORRAR
class btIDebugDraw;
class btDynamicsWorld;


struct myWindow{
    int WW;
    int WH;
    const char* Wname;
    int FPS;
};

class JSONManager;

class RenderManager
{

public:
    RenderManager(int width, int height, const char *windowName, int fps,RenderType renderer);
    void render();
    void finishRender();
    void finishRender3D(); 

    void configure(int width, int height, int fps);
    void setWindow(int type);
    void changerRenderer();
    void addRenderData  (myMesh        myMesh);    
    void addRenderData  (myShape       myShape); 
    void addRenderData  (myCamera      myCamera);
    void addRenderData  (myLight       myLight); 
    void addRenderData  (myImage       myImage); 
    void addRenderData  (myText        myText); 
    void addRenderData  (myParticle    myParticle); 
    bool windowShouldClose();
    myInput updateInput();

    float getDeltaTime();
    float getWindowTime();
    myVector2 getMousePos();
    bool getMouseClick();
    int getFPS();
    myWindow getWindow();

    void setDebugRender(bool debug) {debugRender = debug;};
    void setDebugWorld(btDynamicsWorld* world) {debugWorld.reset(world);};

    btIDebugDraw* getDebugWold() {return debugWorldDrawer.get();};
    void initRender(); //PROVISIONAL

    void loadingAssets(myMesh mesh);
    void quitVideo();
    void setVideoPaused(bool paused) {videoPaused = paused;};

private:
    std::unique_ptr<GraphicsAPI>    renderer{nullptr};
    RenderType                      actualRender{RenderType::Raylib};
    std::unique_ptr<JSONManager>    json3dIds{};
    std::unique_ptr<JSONManager>    json2dIds{};
    
    std::vector<myVector4> lucesPos;
    myWindow                        window{};

    std::unique_ptr<btIDebugDraw>       debugWorldDrawer{};
    std::unique_ptr<btDynamicsWorld>    debugWorld{nullptr};
    bool                                debugRender{false};

    //Gestor de recursos transicionales
    std::vector<myMesh>         dataMesh{};         // Ids de los modelos
    std::vector<myShape>        dataShape{};        // Datos de los rectangulos
    myCamera                    dataCamera{};       // Datos de la cámara
    std::vector<myLight>        dataLights{};       // Datos de la luz (Por hacer)
    std::vector<myImage>        dataImages{};       // Datos de las imagenes
    std::vector<myText>         dataText{};         //Datos de los textos
    std::vector<myParticle>     dataParticle{};         //Datos de los textos

    bool videoStarted = false;
    bool videoPaused = false;

};

#endif