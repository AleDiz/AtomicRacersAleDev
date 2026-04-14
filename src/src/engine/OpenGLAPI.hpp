#pragma once

#ifndef OPENGLAPI_H
#define OPENGLAPI_H

#include "../../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec4.hpp> // Explicitly include the header for glm::vec4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <unordered_map>
#include "GraphicsAPI.hpp"
#include "./graphicEngine/utils/videoPlayer.hpp"

static constexpr int MAX_LIGHTS = 16;


struct Shader3DUniforms {
    GLint uProjection;
    GLint uView;
    GLint uModel;
    GLint uMaterialColor;
    GLint uViewPos;
    GLint uUseTexture;
    GLint uTexture;
};

struct Shader2DUniforms {
    GLint uResolution;
    GLint uColor;
    GLint uImage;
    GLint uPos;
    GLint uSize;
};

struct ShaderOutlineUniforms {
    GLint uProjection;
    GLint uView;
    GLint uModel;
};

struct lightGL {
    glm::vec3 pos;
    glm::vec3 color;     
    float     intensity; 
    float     radius;
};

struct LightData {
    glm::vec4 position;  
    glm::vec4 color;     
};

struct ScoredLight {
    const lightGL* ptr;
    float score;
};

struct LightBlockAligned {
    glm::vec4 positions[MAX_LIGHTS];
    glm::vec4 colors[MAX_LIGHTS];
    int numLights;
    float pad1 = 0.0f, pad2 = 0.0f, pad3 = 0.0f; // padding 16-byte 
};

//struct Font;
struct cameraGL{
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 target;
    glm::vec3 up;
    float fovy;
    int projection;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
};

struct meshGL {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int indexCount;
    GLuint textureID;
    glm::vec3 materialColor;
    float     boundingSphereRadius;
    glm::vec3 localCenter;
};



class OpenGLAPI : public GraphicsAPI
{
public:
    OpenGLAPI(uint16_t width, uint16_t height, const char *windowName, uint16_t fps);
    ~OpenGLAPI();

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
    void clearLights();
    int cont=0;
    // TEXTURES
    void apiLoadTexture(const char* path, uint8_t idTexture) override;
    void apiDrawTextureEx( uint8_t idTexture, myVector2 position, float rotation, float scale, myColor color) override;
    void apiUnloadAllTextures() override;

    //CACHE
    void loadModelCachedScreen(const std::string& path, const std::string& pathT) override;
    std::vector<meshGL> loadModelCached(const std::string& path, const std::string& pathT);
    GLuint loadTextureCached(const std::string& path);


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
    void drawVideoFrame(float x, float y, float scale);
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

    //Font getFont(uint8_t idFont);


    //VBO y VAO
    void initQuad();
    meshGL createCubeTest();
    
private:
    double lastFrameTime = 0.0;
    bool wasPressedLastFrame = false;

    bool didDo3DThisFrame = false;
    double lastTime = 0.0; 

    int   frameCount    = 0;
    float timeAccumulator = 0.0f;
    int   currentFPS      = 0;

    glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f)); 
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);                 
    glm::vec3 ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);

    glm::mat4 projection3D;
    glm::mat4 cameraViewMatrix;
    // ventana;
    GLFWwindow* window;
    // FONTS
    //std::unordered_map<uint8_t, Font> fonts;

    //CACHE
    std::unordered_map<std::string, std::vector<meshGL>> modelCache;
    std::unordered_map<std::string, GLuint> textureCache;
    std::vector<lightGL> lights;

    GLuint quadVAO = 0, quadVBO = 0;

    // CAMERA
    cameraGL openglCamera;

    // TEXTURES 

    // Metodos privados para las texturas
    GLuint loadImage(const char* fileName);
    //Texture2D loadTextureFromImage();
    void unloadImage();
    //void unloadTexture(Texture2D texture);

    //cargar shaders
    unsigned int make_shader(const std::string& vertex_filepath, const std::string& fragment_filepath);
    unsigned int make_module(const std::string& filepath, unsigned int module_type);

    GLuint shader2D, shader3D, shaderDebug, shaderOutline;
    Shader2DUniforms    shader2DUniforms;
    Shader3DUniforms    shader3DUniforms;
    ShaderOutlineUniforms shaderOutlineUniforms;

    GLint modelLoc;      // ubicación de 'uModel'
    GLint materialLoc;   // ubicación de 'uMaterialColor'
    
    // Imagen temporal para cargar texturas
    //Image image;     
    glm::vec3 lightDirWorld = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    // Textura temporal al cargar 
    //Texture2D texture; 

    GLuint defaultVAO;
    GLuint uboLights;

    glm::mat4                projView;
    std::array<glm::vec4, 6> frustumPlanes; // almacenaremos (A,B,C,D) de cada plano
    // ... resto de miembros ...
    // Declaraciones de helper:
    void     extractPlane(int idx, const glm::vec4& row4, const glm::vec4& rowX);
    bool     isSphereInsideFrustum(const glm::vec3& center, float radius) const;
    bool lastShiftState = false;


    //video
    std::unique_ptr<VLCVideoPlayer> m_videoPlayer;
    GLuint m_videoTextureID = 0;
    int m_videoWidth = 0;
    int m_videoHeight = 0;
    bool m_videoLoaded = false;
};


#endif 

