#pragma once

#ifndef RAYLIBAPI_H
#define RAYLIBAPI_H

#include <unordered_map>
#include <string>
#include <chrono>
#include <iostream>
#include <array>



#include "GraphicsAPI.hpp"

#include "raylib.h" //WARNING ALE


class RaylibAPI : public GraphicsAPI
{

public:
    RaylibAPI(uint16_t width, uint16_t height, const char *windowName, uint16_t fps);
    ~RaylibAPI();

    // GENERAL
    void apiSetBackgroundColor(myColor color) override;
    void apiClose() override;
    void apiStartDraw() override;
    void apiFinishDraw() override;
    void apiStartDraw3D() override;
    void apiFinishDraw3D() override;
    bool apiWindowShouldClose() override;

    // CAMERA
    void apiUpdateCamera(myCamera camera) override;

    // LIGHTS
    void apiPutLigth(myVector4 position, myColor color) override;

    // TEXTURES
    void apiLoadTexture(const char* path, uint8_t idTexture) override;
    void apiDrawTextureEx( uint8_t idTexture, myVector2 position, float rotation, float scale, myColor color) override;
    void apiUnloadAllTextures() override;

    //CACHE
    Model loadModelCached(const std::string& path, const std::string& pathT);
    Texture2D loadTextureCached(const std::string& path);

    // DRAWING IN 3D
    void apiDrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, myColor color) override;
    void apiDrawCircle(uint8_t centerX, uint8_t centerY, float radius, myColor color) override;
    void apiDrawObject(myVector3 position, myQuaternion rotation, std::string path3D, std::string pathT, std::string pathN) override;
    void apiDrawCube(myVector3 position, myQuaternion rotation, float width, float height, float length, myColor color) override;
    void apiDrawCollisionShape(myVector3 position, myVector3 size, myColor color) override;

    // DRAWING IN 2D
    void apiLoadFont(uint8_t idFont, const std::string &filePath, uint16_t size) override;
    void apiUnloadFonts() override;
    void apiDrawText(const char* text, float x, float y, uint8_t fontSize, myColor color) override;
    void apiDrawCentredTextWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color) override;
    void apiDrawTxtWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color) override;
    void apiDrawImage(const std::string& src, float x, float y, float scale, myColor col) override;

    // VIDEO
    void apiPlayVideo(const std::string& src, float w, float h,bool loop) override;
    void updateVideo() override;
    void unloadVideo() override;

    // WINDOW MANAGEMENT
    void apiSetWindowMode(int type) override;
    void apiChangeResolution(uint16_t width, uint16_t height) override;

    // GETTERS
    int apiGetWidth() override;
    int apiGetHeight() override;
    int apiGetFPS() override;
    float apiGetDeltaTime() override;
    float apiGetWindowTime() override;
    myVector2 apiGetMouse() override;
    bool apiClickMouse() override;

    // INPUT DETECTIONS
    myInput apiReturnInput() override;

    Font getFont(uint8_t idFont);

    void loadModelCachedScreen(const std::string& path, const std::string& pathT) override;
private:
    // FONTS
    std::unordered_map<uint8_t, Font> fonts;

    //CACHE
    std::unordered_map<std::string, Model> modelCache;
    std::unordered_map<std::string, Texture2D> textureCache;

    // CAMERA
    Camera raylibCamera;

    // TEXTURES 

    // Metodos privados para las texturas
    Image loadImage(const char* fileName);
    Texture2D loadTextureFromImage();
    void unloadImage();
    void unloadTexture(Texture2D texture);
    
    // Imagen temporal para cargar texturas
    Image image;     

    // Textura temporal al cargar 
    Texture2D texture; 
};


#endif