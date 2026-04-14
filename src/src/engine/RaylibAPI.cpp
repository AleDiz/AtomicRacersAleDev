#include "RaylibAPI.hpp"

#include "raylib.h"
#include "rlgl.h"

#include "../util/RenderUtils.hpp"
#include "../util/PowerUpsTypes.hpp"
#include "../util/ComponentKey.hpp"


RaylibAPI::RaylibAPI(uint16_t width, uint16_t height, const char *windowName, uint16_t fps) : raylibCamera()
{
    InitWindow(static_cast<int>(width), static_cast<int>(height), windowName);
    SetTargetFPS(static_cast<int>(fps));
    SetExitKey(KEY_NULL);
}

RaylibAPI::~RaylibAPI() {
    for (auto& entry : modelCache) {
        UnloadModel(entry.second); // Liberar cada modelo cargado
    }
}

// GENERAL
void RaylibAPI::apiSetBackgroundColor(myColor color)
{
    Color raylibColor = {color.r, color.g, color.b, color.a};
    ClearBackground(raylibColor);
}

void RaylibAPI::apiClose()
{
    CloseWindow();
}

void RaylibAPI::apiStartDraw()
{
    BeginDrawing();
}

void RaylibAPI::apiFinishDraw()
{
    EndDrawing();
}

void RaylibAPI::apiStartDraw3D()
{
    BeginMode3D(raylibCamera);
}

void RaylibAPI::apiFinishDraw3D()
{
    EndMode3D();
}

bool RaylibAPI::apiWindowShouldClose()
{
    return WindowShouldClose();
}

// CAMERA
void RaylibAPI::apiUpdateCamera(myCamera camera)
{
    raylibCamera.position = {camera.position.x, camera.position.y, camera.position.z};
    raylibCamera.target = {camera.target.x, camera.target.y, camera.target.z};
    raylibCamera.up = {camera.up.x, camera.up.y, camera.up.z};
    raylibCamera.fovy = camera.fovy;
    raylibCamera.projection = camera.projection;
}

// TEXTURES Legacy
void RaylibAPI::apiLoadTexture(const char* path, uint8_t idTexture){
    
}
void RaylibAPI::apiDrawTextureEx( uint8_t idTexture, myVector2 position, float rotation, float scale, myColor color){
    
}
void RaylibAPI::apiUnloadAllTextures() {
    
}

//CACHES
void RaylibAPI::loadModelCachedScreen(const std::string& path, const std::string& pathT) {
    loadModelCached(path,pathT);
}

Model RaylibAPI::loadModelCached(const std::string& path, const std::string& pathT) {
    // Si el modelo ya está en caché, devolverlo
    if (modelCache.find(path) != modelCache.end()) {
        return modelCache[path];
    }

    // Si no está en caché, cargarlo y guardarlo
    Model model = LoadModel(path.c_str());
    if(pathT != ""){
        Texture2D texture = LoadTexture(pathT.c_str());
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    }
    modelCache[path] = model;
    return model;
}

Texture2D RaylibAPI::loadTextureCached(const std::string& path) {
    // Si la textura ya está en caché, devolverla
    if (textureCache.find(path) != textureCache.end()) {
        return textureCache[path];
    }

    // Si no está en caché, cargar la textura y guardarla
    Image image = LoadImage(path.c_str());
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);  // Ya no necesitamos la imagen en RAM

    textureCache[path] = texture;
    return texture;
}

// DRAWING IN 3D
void RaylibAPI::apiDrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, myColor color)
{
    Color raylibColor = {color.r, color.g, color.b, color.a};
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height), raylibColor);
}

void RaylibAPI::apiDrawCircle(uint8_t centerX, uint8_t centerY, float radius, myColor color)
{
    Color raylibColor = {color.r, color.g, color.b, color.a};
    DrawCircle(static_cast<int>(centerX), static_cast<int>(centerY), radius, raylibColor);
}

void RaylibAPI::apiDrawObject(myVector3 position, myQuaternion rotation, std::string path3D, std::string pathT, std::string pathN)
{
    Model model = loadModelCached(path3D, pathT);
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotation.angle * RAD2DEG, rotation.axis.x, rotation.axis.y, rotation.axis.z);
    DrawModel(model, {0.0f,0.0f,0.0f}, 1.0f, WHITE);
    rlPopMatrix();
}

void RaylibAPI::apiDrawCube(myVector3 position, myQuaternion rotation, float width, float height, float length, myColor color)
{
    Color colorV = {color.r, color.g, color.b, color.a};

    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotation.angle * RAD2DEG, rotation.axis.x, rotation.axis.y, rotation.axis.z);
    DrawCube({0.0f,0.0f,0.0f}, width, height, length, colorV);
    rlPopMatrix();
}

void RaylibAPI::apiDrawCollisionShape(myVector3 position, myVector3 size, myColor color)
{
    Vector3 positionV = {position.x, position.y, position.z};
    Vector3 sizeV = {size.x, size.y, size.z};
    Color colorV = {color.r, color.g, color.b, color.a};
    DrawCubeWires(positionV, sizeV.x, sizeV.y, sizeV.z, colorV);
}

// DRAWING IN 2D
void RaylibAPI::apiLoadFont(uint8_t idFont, const std::string &filePath, uint16_t size)
{
    if (fonts.find(idFont) == fonts.end()) 
    {  // Verifica si la clave ya está en el mapa
        fonts[idFont] = LoadFontEx(filePath.c_str(), static_cast<int>(size), 0, 255);
    }
}

void RaylibAPI::apiUnloadFonts()
{
    for (auto &fontPair : fonts) 
    {
        UnloadFont(fontPair.second);
    }
    fonts.clear();
}

void RaylibAPI::apiDrawText(const char* text, float x, float y, uint8_t fontSize, myColor color)
{
    Color raylibColor = {color.r, color.g, color.b, color.a};
    DrawText(text, static_cast<int>(x), static_cast<int>(y), fontSize, raylibColor);
}

void RaylibAPI::apiDrawCentredTextWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color)
{
    Font font = fonts[idFont];

    Vector2 measure = MeasureTextEx(font, text, fontSize, 1.0f);
    Vector2 position = {static_cast<float>(posX) - measure.x / 2, 
                        static_cast<float>(posY) - measure.y / 2};
    Color raylibColor = {color.r, color.g, color.b, color.a};
    
    DrawTextEx(font, text, position, fontSize, 1.0f, {raylibColor.r, raylibColor.g, raylibColor.b, raylibColor.a});
}

void RaylibAPI::apiDrawTxtWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color)
{
    // Font font = getFont(idFont);
    // Color colorV = {color.r, color.g, color.b, color.a};
    // Vector2 position = {static_cast<float>(posX), 
    //                     static_cast<float>(posY)};

    // DrawTextEx(font, text, position, fontSize, 1.0f, colorV);
}

void RaylibAPI::apiDrawImage(const std::string& src, float x, float y, float scale, myColor col)
{
    // Cargar imagen desde el archivo
    Texture2D texture = loadTextureCached(src);

    Vector2 position = { x, y };  // No aplicamos rotación
    Color tint {col.r,col.g, col.b, col.a};

    DrawTextureEx(texture, position, 0.0f, scale, tint);
}

// WINDOW MANAGEMENT
void RaylibAPI::apiSetWindowMode(int type)
{
    ToggleFullscreen();
}

void RaylibAPI::apiChangeResolution(uint16_t width, uint16_t height)
{
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
    }

    SetWindowSize(static_cast<int>(width), static_cast<int>(height));
}

// GETTERS
int RaylibAPI::apiGetWidth()
{
    return GetRenderWidth();
}

int RaylibAPI::apiGetHeight()
{
    return GetRenderHeight();
}

int RaylibAPI::apiGetFPS()
{
    return GetFPS();
}
float RaylibAPI::apiGetDeltaTime(){
    return GetFrameTime();
}
float RaylibAPI::apiGetWindowTime(){
    return GetTime();
}
//Devuelve la posicion del raton dentro de la ventana
myVector2 RaylibAPI::apiGetMouse(){
    auto mousePos  = GetMousePosition();
    return myVector2{mousePos.x, mousePos.y};
}

bool RaylibAPI::apiClickMouse(){
    return IsMouseButtonReleased(MouseButton::MOUSE_BUTTON_LEFT);
}

void RaylibAPI::apiPutLigth(myVector4 position, myColor color)
{
    
}   




// INPUT DETECTIONS
myInput RaylibAPI::apiReturnInput()
{
    myInput structInput {};
    int lastInput {0};
    float RJ {0.0f};    
    float LJ {0.0f};
    float R2 {0.0f};
    float L2 {0.0f};

    // Detectar estado del gamepad (usamos el primer joystick como ejemplo)
    if (IsGamepadAvailable(0)) {
        // Botones del gamepad
        if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {  // A o X (Xbox) / Cross (PS)
            lastInput |= ComponentKey::COMPONENT_ENTER;
        }
        if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) { // B o Circle (PS) / B (Xbox)
            lastInput |= ComponentKey::COMPONENT_ENTER;
        }
        if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {    // Y o Triangle (PS) / Y (Xbox)
            lastInput |= ComponentKey::COMPONENT_ENTER;
        }
        if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {  // X o Square (PS) / X (Xbox)
            lastInput |= ComponentKey::COMPONENT_ENTER;
        }

        // Bumper izquierdo y derecho
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {   // L1 (PS) / LB (Xbox)
            lastInput |= ComponentKey::COMPONENT_SP;
        }
        if (IsGamepadButtonReleased(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {  // R1 (PS) / RB (Xbox)
            lastInput |= ComponentKey::COMPONENT_ENTER;
        }

        // Ejes del gamepad
        float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        if (axisX > 0.06f) {
            lastInput |= ComponentKey::COMPONENT_D;
            LJ = axisX;
        }
        if (axisX < -0.06f) {
            lastInput |= ComponentKey::COMPONENT_A;
            LJ = axisX;
        }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.06f) {
            lastInput |= ComponentKey::COMPONENT_S;
            L2 = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER);
        }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.06f) {
            lastInput |= ComponentKey::COMPONENT_W;
            R2 = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER);
        }

 
    }else {
        // Si no hay gamepad, usar teclas del teclado
        if (IsKeyDown(KEY_W)) {
            lastInput |= ComponentKey::COMPONENT_W;
        }
        if (IsKeyDown(KEY_S)) {
            lastInput |= ComponentKey::COMPONENT_S;
        }
        if (IsKeyDown(KEY_A)) {
            lastInput |= ComponentKey::COMPONENT_A;
        }
        if (IsKeyDown(KEY_D)) {
            lastInput |= ComponentKey::COMPONENT_D;
        }
        if (IsKeyDown(KEY_SPACE)) {
            lastInput |= ComponentKey::COMPONENT_SP;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            lastInput |= ComponentKey::COMPONENT_CTRL;
        }
        if (IsKeyReleased(KEY_LEFT_SHIFT)) {
            lastInput |= ComponentKey::COMPONENT_SF;
        }
        if (IsKeyReleased(KEY_ENTER)) {
            lastInput |= ComponentKey::COMPONENT_ENTER;
        }
        if (IsKeyReleased(KEY_ESCAPE)) {
            lastInput |= ComponentKey::COMPONENT_ESC;
        }
    }

    structInput.inputMask = lastInput;
    structInput.L2 = L2;
    structInput.R2 = R2;
    structInput.LJ = LJ;
    structInput.RJ = RJ;

    return structInput;
}

void RaylibAPI::apiPlayVideo(const std::string& path, float w, float h, bool loop)
{
    
}

void RaylibAPI::updateVideo()
{
    
}

void RaylibAPI::unloadVideo()
{
    
}