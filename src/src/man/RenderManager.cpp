#include "RenderManager.hpp"

#include "../util/JsonManager.hpp"
#include "../engine/GraphicsAPI.hpp"
#include "../engine/RaylibAPI.hpp"
#include "../engine/OpenGLAPI.hpp"

#include"../engine/RaylibDebugDrawer.hpp"

constexpr const char path3D[] = "assets/JSON/renderManager/grafico.json";
constexpr const char path2D[] = "assets/JSON/renderManager/sprites.json";

RenderManager::RenderManager(int width, int height, const char *windowName, int fps,RenderType rendererT)
    : actualRender(rendererT)
{
    //Creando la ventana
    window.WW = width;
    window.WH = height;
    window.Wname = windowName;
    window.FPS = fps;

    json3dIds = std::make_unique<JSONManager>();
    std::string path3D_str = path3D;
    json3dIds->loadJSON(path3D_str);

    json2dIds = std::make_unique<JSONManager>();
    std::string path2D_str = path2D;
    json2dIds->loadJSON(path2D_str);

    

    //Creando el GraphicAPI pertinente
    if(rendererT == RenderType::Raylib)
    {
        renderer = std::make_unique<RaylibAPI>(width, height, windowName, fps);
        debugWorldDrawer = std::make_unique<RaylibDebugDrawer>();
    }
    else
    {
        renderer = std::make_unique<OpenGLAPI>(width, height, windowName, fps);
        addRenderData(myLight{myVector3{0, 0, 0}, myColor{255, 255, 255, 255}, 1.0f});
    }

    
}

bool RenderManager::windowShouldClose(){
    return renderer->apiWindowShouldClose();
}

void RenderManager::loadingAssets(myMesh mesh){
    std::array<std::string, 3> direcciones;

    json3dIds->getInfoID(mesh.id, direcciones);
    renderer->loadModelCachedScreen(direcciones[0], direcciones[1]);
}

// JSONManager lucesJSON;
// lucesJSON.loadJSON("assets/JSON/luces/lucesPositions.json");
// lucesJSON.leerposiciones("assets/JSON/luces/lucesPositions.json", lucesPos);

// for(int i = 0; i < lucesPos.size(); i++){
//     renderer->apiPutLigth(lucesPos[i], myColor{255, 255, 255, 255});
// }

void RenderManager::render(){
    
    renderer->apiUpdateCamera(dataCamera);
    
    renderer->apiSetBackgroundColor(myColor{ 0, 0, 0, 255 });

    renderer->apiStartDraw();
    //Pintamos los 2Ds
    for (myImage& image : dataImages){
        std::array<std::string, 4> direcciones;
        json2dIds->getInfo2D(image.id, direcciones);
        if(std::stof(direcciones[2])==1920.0f && std::stof(direcciones[3])==1080.0f){

            float originalWidth = std::stof(direcciones[2]);
            float originalHeight = std::stof(direcciones[3]);
            float scaleX = 1.0f;
            float scaleY = 1.0f;
            
            if(!(window.WW == 1920 && window.WH == 1080)){
                scaleX = static_cast<float>(window.WW) / 1920.0f;
                scaleY = static_cast<float>(window.WH) / 1080.0f;
                image.position.x *= scaleX;
                image.position.y *= scaleY;
                
            }

            if (image.id >1000 && !videoStarted)
            {
            
                bool bucle = std::stof(direcciones[1]) == 1;
                renderer->apiPlayVideo(direcciones[0], originalWidth * scaleX, originalHeight * scaleX, bucle);
                videoStarted = true;
                videoPaused = !bucle;
                
            }
            
            
            if(image.hover){
                renderer->apiDrawImage(direcciones[1], image.position.x, image.position.y, scaleX, image.color);
            }else{
                renderer->apiDrawImage(direcciones[0], image.position.x, image.position.y, scaleX, image.color);
            }
        }
    }
    if (!videoPaused)
    {
        renderer->updateVideo();
    }
    
    
    
    //Comenzamos el dibujado

    renderer->apiStartDraw3D();
    //Primero vemos los id y lo traducimos a path, tras esto se le pasa al api correspondiente junto con su posición y rotación
    for (myMesh& mesh : dataMesh){
        std::array<std::string, 3> direcciones;

        json3dIds->getInfoID(mesh.id, direcciones);
        renderer->apiDrawObject(mesh.position, mesh.rotation, direcciones[0], direcciones[1], direcciones[2]);
    }
    for (myParticle& particle : dataParticle){
        std::array<std::string, 3> direcciones;

        json3dIds->getInfoID(particle.id, direcciones);
        renderer->apiDrawObject(particle.position, particle.rotation, direcciones[0], direcciones[1], direcciones[2]);
    }
    for (myShape& shape : dataShape){
        renderer->apiDrawCube(shape.position, shape.rotation ,shape.width,shape.height, shape.length, shape.color);
    }
    //Despues se pasan las luces al api
    for (myLight& light : dataLights){
        //renderer->apiPutLigth(pos, color, intensidad) WARNING ALE
    }

    if(debugRender){
        debugWorld->debugDrawWorld();
    }

    finishRender3D();
    
    //Actualizamos la camara

    renderer->apiStartDraw();
    //Pintamos los 2Ds
    for (myImage& image : dataImages){
        std::array<std::string, 4> direcciones;
        json2dIds->getInfo2D(image.id, direcciones);
        if(std::stof(direcciones[2])!=1920.0f && std::stof(direcciones[3])!=1080.0f){
            float originalWidth = std::stof(direcciones[2]);
            float originalHeight = std::stof(direcciones[3]);
            float scaleX = 1.0f;
            float scaleY = 1.0f;
            
            if(!(window.WW == 1920 && window.WH == 1080)){
                scaleX = static_cast<float>(window.WW) / 1920.0f;
                scaleY = static_cast<float>(window.WH) / 1080.0f;
                image.position.x *= scaleX;
                image.position.y *= scaleY;
                
            }
            
            if(image.hover){
                renderer->apiDrawImage(direcciones[1], image.position.x, image.position.y, scaleX, image.color);
            }else{
                renderer->apiDrawImage(direcciones[0], image.position.x, image.position.y, scaleX, image.color);
            }
        }
    }

    //Pinto texto
    for (myText& text : dataText){
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        
        if(!(window.WW == 1920 && window.WH == 1080)){
            scaleX = static_cast<float>(window.WW) / 1920.0f;
            scaleY = static_cast<float>(window.WH) / 1080.0f;

            text.pos.x *= scaleX;
            text.pos.y *= scaleY;

            text.size *= scaleX;
        }
        renderer->apiDrawText(text.texto.c_str(),text.pos.x, text.pos.y, text.size, text.color);
    }

    

    finishRender();
}

void RenderManager::finishRender(){
    renderer->apiFinishDraw();
    dataMesh.clear();    
    dataShape.clear();
    //dataLights.clear();
    dataImages.clear();
    dataText.clear();
    dataParticle.clear();
}

void RenderManager::finishRender3D(){
    renderer->apiFinishDraw3D();
}

void RenderManager::configure(int WW, int WH, int fps){
    window.WW = WW;
    window.WH = WH;
    window.FPS = fps;

    renderer->apiChangeResolution(WW, WH);
    //Se podrian cambiar los fps tmb
}

void RenderManager::setWindow(int type){
    renderer->apiSetWindowMode(type);
}
void RenderManager::changerRenderer(){
    renderer->apiClose();
    if (actualRender == RenderType::Raylib) {
        
        renderer = std::make_unique<OpenGLAPI>(window.WW, window.WH, window.Wname, window.FPS);
        actualRender = RenderType::OpenGL;
        addRenderData(myLight{myVector3{0, 0, 0}, myColor{255, 255, 255, 255}, 1.0f});
    } else if (actualRender == RenderType::OpenGL) {
        renderer = std::make_unique<RaylibAPI>(window.WW, window.WH, window.Wname, window.FPS);
        actualRender = RenderType::Raylib;

    }

    videoStarted = false;
}


void RenderManager::addRenderData(myMesh myMesh)
{
    dataMesh.push_back(myMesh);
}   

void RenderManager::addRenderData(myShape myShape)
{
    dataShape.push_back(myShape);
}

void RenderManager::addRenderData(myCamera myCamera)
{
    dataCamera.fovy = myCamera.fovy;
    dataCamera.position = myCamera.position;
    dataCamera.projection = myCamera.projection;
    dataCamera.target = myCamera.target;
    dataCamera.up = myCamera.up;
}

void RenderManager::addRenderData(myLight myLight)
{

    //QUITARLAS CUANDO ESTE LA PANTALLA DE CARGA
    JSONManager lucesJSON;
    lucesJSON.loadJSON("assets/JSON/luces/lucesPositions.json");
    lucesJSON.leerposiciones("assets/JSON/luces/lucesPositions.json", lucesPos);
    
    for(int i = 0; i < lucesPos.size(); i++){
        renderer->apiPutLigth(lucesPos[i], myColor{255, 255, 255, 255});
    }
}

void RenderManager::addRenderData(myImage myImage)
{
    dataImages.push_back(myImage);
}

void RenderManager::addRenderData(myText myText)
{
    dataText.push_back(myText);
}

void RenderManager::addRenderData(myParticle myParticle)
{
    dataParticle.push_back(myParticle);
}

myInput RenderManager::updateInput(){
    return renderer->apiReturnInput();
}

void RenderManager::initRender(){
    renderer->apiStartDraw();
    renderer->apiStartDraw3D();
}

myVector2 RenderManager::getMousePos(){
    //Devuelve la posicion del raton en la pantalla
    return renderer->apiGetMouse();
}

bool RenderManager::getMouseClick(){
    return renderer->apiClickMouse();
}

float RenderManager::getDeltaTime(){
    return renderer->apiGetDeltaTime();
}

float RenderManager::getWindowTime(){
    return renderer->apiGetWindowTime();
}

int RenderManager::getFPS(){
    return renderer->apiGetFPS();
}

myWindow RenderManager::getWindow(){
    return window;
}

void RenderManager::quitVideo(){
    renderer->unloadVideo();
    videoStarted = false;
}
