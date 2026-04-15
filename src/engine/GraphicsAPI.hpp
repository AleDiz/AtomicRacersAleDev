#ifndef GRAPHICS_API_H
#define GRAPHICS_API_H

#include <string>
#include <chrono>

#include "../util/RenderUtils.hpp"

class GraphicsAPI {
public:
    virtual ~GraphicsAPI() = default;

    // GENERAL
    virtual void apiSetBackgroundColor(myColor color) = 0 ;
    virtual void apiClose() = 0;
    virtual void apiStartDraw() = 0;
    virtual void apiFinishDraw() = 0;
    virtual void apiStartDraw3D() = 0;
    virtual void apiFinishDraw3D() = 0;
    virtual bool apiWindowShouldClose() = 0;

    // CAMERA
    virtual void apiUpdateCamera(myCamera camera) = 0;

    // LIGHTS
    virtual void apiPutLigth(myVector4 position, myColor color) = 0;

    // TEXTURES
    virtual void apiLoadTexture(const char* path, uint8_t idTexture) = 0;
    virtual void apiDrawTextureEx( uint8_t idTexture, myVector2 position, float rotation, float scale, myColor color) = 0;
    virtual void apiUnloadAllTextures() = 0;

    // DRAWING IN 3D
    virtual void apiDrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, myColor color) = 0;
    virtual void apiDrawCircle(uint8_t centerX, uint8_t centerY, float radius, myColor color) = 0;
    virtual void apiDrawObject(myVector3 position, myQuaternion rotation, std::string path3D, std::string pathT, std::string pathN) = 0;
    virtual void apiDrawCube(myVector3 position, myQuaternion rotation, float width, float height, float length, myColor color) = 0;
    virtual void apiDrawCollisionShape(myVector3 position, myVector3 size, myColor color) = 0;

    virtual void loadModelCachedScreen(const std::string& path, const std::string& pathT) = 0;
    virtual void updateVideo() = 0;
    virtual void unloadVideo() = 0;

    // DRAWING IN 2D - HAY QUE REVISARLO
    virtual void apiLoadFont(uint8_t idFont, const std::string &filePath, uint16_t size) = 0;
    virtual void apiUnloadFonts() = 0;
    virtual void apiDrawText(const char* text, float x, float y, uint8_t fontSize, myColor color) = 0;
    virtual void apiDrawCentredTextWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color) = 0;
    virtual void apiDrawTxtWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color) = 0;
    virtual void apiDrawImage(const std::string& src, float x, float y, float scale, myColor col) = 0;
    virtual void apiPlayVideo(const std::string& src, float w, float h,bool loop) = 0;

    // WINDOW MANAGEMENT
    virtual void apiSetWindowMode(int type) = 0;
    virtual void apiChangeResolution(uint16_t width, uint16_t height) = 0;

    // GETTERS
    virtual int apiGetWidth() = 0;
    virtual int apiGetHeight() = 0;
    virtual int apiGetFPS() = 0;
    virtual float apiGetDeltaTime() = 0;
    virtual float apiGetWindowTime() = 0;
    virtual myVector2 apiGetMouse() = 0;
    virtual bool apiClickMouse() = 0;

    // INPUT DETECTIONS
    virtual myInput apiReturnInput() = 0;
    
};

#endif // GRAPHICS_API_H
