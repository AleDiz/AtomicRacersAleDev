#pragma once

#include <unordered_map>
#include <string>
#include "raylib.h"

class TextureManager {
public:
    // Cargar una textura usando su ruta como identificador
    void loadTexture(const char* path);

    // Obtener una textura cargada usando su ruta
    Texture2D getTexture(const char* path);
    
    // Dibujar una textura usando su ruta
    void drawTextureEx(const char* path, Vector2 position, float rotation, float scale, Color tint);

    // Descargar todas las texturas cargadas
    void unloadAllTextures();

private:
    // Métodos internos
    Image loadImage(const char* fileName);
    Texture2D loadTextureFromImage();
    void unloadImage();
    void unloadTexture(Texture2D texture);

    // Almacén de texturas: ruta -> textura
    std::unordered_map<std::string, Texture2D> textures;
    
    Image image;      // Imagen temporal para cargar texturas
    Texture2D texture; // Textura temporal al cargar
};
