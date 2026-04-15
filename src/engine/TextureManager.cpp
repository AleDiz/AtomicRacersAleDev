#include "TextureManager.hpp"
#include <iostream>
#include <filesystem>

void TextureManager::loadTexture(const char* path) {
    // Primero normalizas la ruta (si es necesario)

    // Si no está ya cargada, carga la textura y almacénala
    if (textures.find(path) == textures.end()) {
        // Cargar la imagen
        image = loadImage(path);
        texture = loadTextureFromImage();
        unloadImage();  // Descartamos la imagen, solo dejamos la textura

        // Almacenamos la textura en el mapa
        textures[path] = texture;
    }
}

Texture2D TextureManager::getTexture(const char* path) {
    
    auto it = textures.find(path);
    if (it == textures.end()) {
    }
    return it->second;
}

// Dibujar textura usando su ruta
void TextureManager::drawTextureEx(const char* path, Vector2 position, float rotation, float scale, Color tint) {
    
    auto it = textures.find(path);
    if (it != textures.end()) {
        DrawTextureEx(it->second, position, rotation, scale, tint);
    }
}

void TextureManager::unloadAllTextures() {
    if (textures.size() > 0)
    {
        for (auto& entry : textures) {
            UnloadTexture(entry.second);
        }
        textures.clear();
    }
}

// Métodos internos
Image TextureManager::loadImage(const char* fileName) {
    return LoadImage(fileName);
}

Texture2D TextureManager::loadTextureFromImage() {
    return LoadTextureFromImage(image);
}

void TextureManager::unloadImage() {
    UnloadImage(image);
}

void TextureManager::unloadTexture(Texture2D texture) {
    UnloadTexture(texture);
}
