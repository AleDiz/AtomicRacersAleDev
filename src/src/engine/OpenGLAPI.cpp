#include "OpenGLAPI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#include "./graphicEngine/utils/videoPlayer.hpp"


#include "../util/RenderUtils.hpp"
#include "../util/PowerUpsTypes.hpp"
#include "../util/ComponentKey.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>


#include <GL/gl.h>
#include <cstdio>

   

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Libreria de carga de imagenes
#define STB_IMAGE_IMPLEMENTATION
#include "./graphicEngine/utils/stb_image.h"

//crea un modulo de shader
//Shader program es el nombre completo y los shader modules son partes individuales del shader program
//como vertex shader y fragment shader
unsigned int OpenGLAPI::make_module(const std::string& filepath, unsigned int module_type)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error abriendo shader: " << filepath << std::endl;
        return 0;
    }

    std::stringstream bufferedLines;
    bufferedLines << file.rdbuf();
    file.close();

    std::string shaderSource = bufferedLines.str();
    const char* shaderSrc = shaderSource.c_str();

    unsigned int shader = glCreateShader(module_type);
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_FAILED (" 
                  << filepath << "):\n" 
                  << infoLog << std::endl;
    }

    return shader;
}

unsigned int OpenGLAPI::make_shader(const std::string& vertex_filepath,
                                    const std::string& fragment_filepath)
{
    
    unsigned int vertShader = make_module(vertex_filepath,   GL_VERTEX_SHADER);
    unsigned int fragShader = make_module(fragment_filepath, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    if (vertShader) glAttachShader(program, vertShader);
    if (fragShader) glAttachShader(program, fragShader);

    
    glLinkProgram(program);

    
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_FAILED:\n" 
                  << infoLog << std::endl;
    }

    
    glValidateProgram(program);
    GLint validated = 0;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validated);
    if (!validated) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_VALIDATION_FAILED:\n" 
                  << infoLog << std::endl;
    }

    
    if (vertShader) glDeleteShader(vertShader);
    if (fragShader) glDeleteShader(fragShader);

    return program;
}

OpenGLAPI::OpenGLAPI(uint16_t width, uint16_t height, const char *windowName, uint16_t fps)
{
    if (!glfwInit()) {
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // en caso de hacer port a mac  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE) por si lo hacemos mas adelante por nuestra cuenta
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Atomic Racers", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return;
    }
        // Aquí recreas y compruebas shader2D
    shader2D = make_shader("assets/shaders/vertex.txt",
                        "assets/shaders/fragment.txt");

    GLint linked = 0;

    shader2DUniforms.uResolution = glGetUniformLocation(shader2D, "uResolution");
    shader2DUniforms.uColor      = glGetUniformLocation(shader2D, "uColor");
    shader2DUniforms.uImage      = glGetUniformLocation(shader2D, "image");
    shader2DUniforms.uPos        = glGetUniformLocation(shader2D, "uPos");
    shader2DUniforms.uSize       = glGetUniformLocation(shader2D, "uSize");

    glGetProgramiv(shader2D, GL_LINK_STATUS, &linked);

    // activa el v-sync
    //Si vale 0 no se activa - Si vale 1 los fps se limitan a los hz del monitor - si vale 2 a la mitad de los hz del monitor
    glfwSwapInterval(0); 
    didDo3DThisFrame = true;
    glUseProgram(shader2D);
    GLenum e = glGetError();

    glGenBuffers(1, &uboLights);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockAligned), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboLights); // Binding 0 coincide con shader
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    glViewport(0, 0, width, height); // establece el tamaño de la ventana, se llama cuando la reescalemos porque define el area de renderizado
    //shader2D = make_shader("assets/shaders/vertex.txt", "assets/shaders/fragment.txt");
    shader3D = make_shader("assets/shaders/debug3DV.txt", "assets/shaders/debug3DF.txt");

    shader3DUniforms.uProjection    = glGetUniformLocation(shader3D, "uProjection");
    shader3DUniforms.uView          = glGetUniformLocation(shader3D, "uView");
    shader3DUniforms.uModel         = glGetUniformLocation(shader3D, "uModel");
    shader3DUniforms.uMaterialColor = glGetUniformLocation(shader3D, "uMaterialColor");
    shader3DUniforms.uViewPos       = glGetUniformLocation(shader3D, "uViewPos");
    shader3DUniforms.uUseTexture    = glGetUniformLocation(shader3D, "uUseTexture");
    shader3DUniforms.uTexture       = glGetUniformLocation(shader3D, "uTexture");

    shaderDebug = make_shader("assets/shaders/debugvertex.txt", "assets/shaders/debugfragment.txt");

    
    shaderOutline = make_shader("assets/shaders/shaderOutlineV.txt", "assets/shaders/shaderOutlineF.txt");
    shaderOutlineUniforms.uProjection = glGetUniformLocation(shaderOutline, "uProjection");
    shaderOutlineUniforms.uView       = glGetUniformLocation(shaderOutline, "uView");
    shaderOutlineUniforms.uModel      = glGetUniformLocation(shaderOutline, "uModel");

    /*
    shaderDebugUniforms.uProjection = glGetUniformLocation(shaderDebug, "uProjection");
    shaderDebugUniforms.uView       = glGetUniformLocation(shaderDebug, "uView");
    shaderDebugUniforms.uModel      = glGetUniformLocation(shaderDebug, "uModel");
    */
   
}

OpenGLAPI::~OpenGLAPI()
{
    if (m_videoPlayer) {
        m_videoPlayer->stop();
        m_videoPlayer.reset(); // Libera el recurso correctamente
    }
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void OpenGLAPI::apiSetBackgroundColor(myColor color)
{
    glClearColor(color.r/255, color.g/255, color.b/255, color.a/255);
    
}

void OpenGLAPI::apiClose()
{
    clearLights();
    glfwTerminate();
}

void OpenGLAPI::apiStartDraw()
{
    int w,h; glfwGetFramebufferSize(window,&w,&h);
    glViewport(0,0,w,h);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);


    if (!didDo3DThisFrame) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    

    glUseProgram(shader2D);
    glUniform2f(shader2DUniforms.uResolution, float(w), float(h));
}

void OpenGLAPI::apiFinishDraw()
{
    didDo3DThisFrame = false;
    glfwSwapBuffers(window);
    glfwPollEvents();

    float dt = apiGetDeltaTime();
    frameCount++;
    timeAccumulator += dt;

    if (timeAccumulator >= 1.0f) {
        currentFPS    = frameCount;
        frameCount    = 0;
        timeAccumulator -= 1.0f;
    }
}

void OpenGLAPI::apiStartDraw3D()
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    didDo3DThisFrame = true;
    
    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);      
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);

    //Calcular projection y view
    projection3D = glm::perspective(glm::radians(openglCamera.fovy),
                                    float(w)/float(h), 0.01f, 5000.0f);
    cameraViewMatrix = glm::lookAt(openglCamera.position,
                                   openglCamera.position + openglCamera.front,
                                   openglCamera.up);

    glm::mat4 clip = projection3D * cameraViewMatrix;

    // extraer filas
    glm::vec4 row0 = glm::vec4(clip[0][0], clip[1][0], clip[2][0], clip[3][0]);
    glm::vec4 row1 = glm::vec4(clip[0][1], clip[1][1], clip[2][1], clip[3][1]);
    glm::vec4 row2 = glm::vec4(clip[0][2], clip[1][2], clip[2][2], clip[3][2]);
    glm::vec4 row3 = glm::vec4(clip[0][3], clip[1][3], clip[2][3], clip[3][3]);

    // ahora sí, extraer planos
    extractPlane(0, row3,  row0);   // left
    extractPlane(1, row3, -row0);   // right
    extractPlane(2, row3,  row1);   // bottom
    extractPlane(3, row3, -row1);   // top
    extractPlane(4, row3,  row2);   // near
    extractPlane(5, row3, -row2);   // far

    
    // ----- Enviar a shader principal (shader3D) -----
    glUseProgram(shader3D);
    glUniformMatrix4fv(shader3DUniforms.uProjection, 1, GL_FALSE, glm::value_ptr(projection3D));
    glUniformMatrix4fv(shader3DUniforms.uView,       1, GL_FALSE, glm::value_ptr(cameraViewMatrix));
    //LUCES UBO
    std::vector<ScoredLight> scored;

    const float maxLightDistance = 200.0f;
    const float maxDistSq = maxLightDistance * maxLightDistance;
    int lughtCount = 0;
    for (const lightGL& light : lights) {
        float d2 = glm::distance2(light.pos, openglCamera.position);
        bool closeEnough = d2 < maxDistSq;
        bool insideFrustum = isSphereInsideFrustum(light.pos, light.radius * 1.5f);

        if (closeEnough || insideFrustum) {
            float score = d2;
            if (!insideFrustum)
                score += 1e6f; // penaliza luces fuera del frustum
            scored.push_back({ &light, score });
            lughtCount++;
        }
    }
    // Ordena por score (distancia + penalización)
    std::sort(scored.begin(), scored.end(), [](const ScoredLight& a, const ScoredLight& b) {
        return a.score < b.score;
    });

    // Enviar las luces seleccionadas al UBO
    LightBlockAligned block = {};
    int numLights = std::min((int)scored.size(), MAX_LIGHTS);
    for (int i = 0; i < numLights; ++i) {
        const lightGL& light = *scored[i].ptr;
        block.positions[i] = glm::vec4(light.pos, 0.0f);
        block.colors[i]    = glm::vec4(light.color * light.intensity, 1.0f);
    }
    block.numLights = numLights;

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBlockAligned), &block);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLAPI::apiFinishDraw3D()
{
    //glDisable(GL_DEPTH_TEST);
}

bool OpenGLAPI::apiWindowShouldClose()
{
    return glfwWindowShouldClose(window);
}

void OpenGLAPI::apiUpdateCamera(myCamera camera)
{
   
    openglCamera.position = {
        camera.position.x,
        camera.position.y,
        camera.position.z
    };

    // Front (target - position) 
    glm::vec3 target = {
        camera.target.x,
        camera.target.y,
        camera.target.z
    };
    openglCamera.front = glm::normalize(target - openglCamera.position);

    
    openglCamera.up = {
        camera.up.x,
        camera.up.y,
        camera.up.z
    };

    openglCamera.fovy       = camera.fovy;
    openglCamera.projection = camera.projection;
}

void OpenGLAPI::apiLoadTexture(const char* path, uint8_t idTexture)
{
    //image = loadImage(path);
    //texture = loadTextureFromImage();
    //unloadImage();
    //textures[idTexture] = texture;
}

void OpenGLAPI::apiDrawTextureEx( uint8_t idTexture, myVector2 position, float rotation, float scale, myColor color)
{

}

void OpenGLAPI::apiUnloadAllTextures()
{

}

void OpenGLAPI::apiSetWindowMode(int type)
{
    // Si es tipo 1 que sea full screen si es tipo 0 que sea ventana
    if (type == 1)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        GLFWmonitor* monitor = glfwGetWindowMonitor(window);
        if (!monitor) {
            monitor = glfwGetPrimaryMonitor();
        }
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        int monitorX, monitorY;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);
        int windowX = monitorX + (mode->width - apiGetWidth()) / 2;
        int windowY = monitorY + (mode->height - apiGetHeight()) / 2;
        glfwSetWindowMonitor(window, nullptr, windowX, windowY, apiGetWidth(), apiGetHeight(), GLFW_DONT_CARE);
    }
}

void OpenGLAPI::apiChangeResolution(uint16_t width, uint16_t height)
{
    if (window)
    {
        glfwSetWindowSize(window, width, height);
        glViewport(0, 0, width, height);
        
    }
}

// GETTERS
int OpenGLAPI::apiGetWidth()
{  
    int width;
    glfwGetWindowSize(window, &width, nullptr);
    return width;
}

int OpenGLAPI::apiGetHeight()
{
    int height;
    glfwGetWindowSize(window, nullptr, &height);
    return height;
}

int OpenGLAPI::apiGetFPS()
{
    return currentFPS;
}

GLuint OpenGLAPI::loadTextureCached(const std::string& path)
{
        auto it = textureCache.find(path);
        if (it != textureCache.end()) {
            return it->second;
        }

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(),
                                        &width, &height,
                                        &nrChannels, 4);
        if (!data) {
            return 0;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0,
                    GL_RGBA, width, height,
                    0, GL_RGBA, GL_UNSIGNED_BYTE,
                    data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);

        textureCache[path] = textureID;
        return textureID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void OpenGLAPI::apiDrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, myColor color)
{
    
}
void OpenGLAPI::apiDrawCircle(uint8_t centerX, uint8_t centerY, float radius, myColor color)
{
    
}
void OpenGLAPI::apiDrawObject(myVector3 position,
                              myQuaternion rotation,
                              std::string path3D,
                              std::string pathT,
                              std::string pathN)
{
    auto meshes = loadModelCached(path3D, pathT);
    if (meshes.empty()) return;

    int drawnCount  = 0;
    int culledCount = 0;

    
    for (const meshGL& mesh : meshes) {
 
        glm::vec3 worldCenter = {
            position.x + mesh.localCenter.x,
            position.y + mesh.localCenter.y,
            position.z + mesh.localCenter.z
        };
        // Frustum culling
        if (!isSphereInsideFrustum(worldCenter, mesh.boundingSphereRadius)) {
            culledCount++;
            continue;
        }
        drawnCount++;
        glm::mat4 model =
                        glm::translate(glm::mat4(1.0f),
                                    glm::vec3(position.x,
                                                position.y,
                                                position.z)) *
                        glm::toMat4(glm::angleAxis(
                            rotation.angle,
                            glm::normalize(glm::vec3(
                                rotation.axis.x,
                                rotation.axis.y,
                                rotation.axis.z)))
                        );
                        
        //--------------- calculos outliner -----------------
        float baseThickness = 0.12f;
        float minThickness  = 0.01f;
        float t = glm::clamp(mesh.boundingSphereRadius / 10.0f, 0.0f, 1.0f);
        float adjustedThickness = glm::mix(baseThickness, minThickness, t);
        float scaleFactor = 1.0f + adjustedThickness;

        
        glm::vec3 center = mesh.localCenter;

        
        glm::mat4 scaleAroundCenter = glm::translate(glm::mat4(1.0f), center) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor)) *
                                    glm::translate(glm::mat4(1.0f), -center);

        
        glm::mat4 scaledModel = model * scaleAroundCenter;

        // -------------------  DIBUJAR OUTLINER PRIMERO ------------------
        glUseProgram(shaderOutline);

        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL); 
        glDepthMask(GL_FALSE); 

        glUniformMatrix4fv(shaderOutlineUniforms.uModel,      1, GL_FALSE, glm::value_ptr(scaledModel));
        glUniformMatrix4fv(shaderOutlineUniforms.uView,       1, GL_FALSE, glm::value_ptr(cameraViewMatrix));
        glUniformMatrix4fv(shaderOutlineUniforms.uProjection, 1, GL_FALSE, glm::value_ptr(projection3D));
        glCullFace(GL_FRONT);
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glCullFace(GL_BACK);

       
        glDepthMask(GL_TRUE);

        // ------------------- DIBUJAR MODELO NORMAL ------------------
        glUseProgram(shader3D);
        glUniform3fv(shader3DUniforms.uViewPos, 1, glm::value_ptr(openglCamera.position));
        glUniformMatrix4fv(shader3DUniforms.uModel, 1, GL_FALSE, glm::value_ptr(model));

        bool hasTex = (mesh.textureID != 0);
        glUniform1i(shader3DUniforms.uUseTexture, hasTex ? 1 : 0);
        if (hasTex) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.textureID);
            glUniform1i(shader3DUniforms.uTexture, 0);  
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUniform3fv(shader3DUniforms.uMaterialColor, 1, glm::value_ptr(mesh.materialColor));
        }

        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
            }
}
void OpenGLAPI::apiDrawCube(myVector3 position, myQuaternion rotation, float width, float height, float length, myColor color)
{
    
}
void OpenGLAPI::apiDrawCollisionShape(myVector3 position, myVector3 size, myColor color)
{
    
}
void OpenGLAPI::apiLoadFont(uint8_t idFont, const std::string &filePath, uint16_t size)
{
    
}
void OpenGLAPI::apiUnloadFonts()
{
    
}
void OpenGLAPI::apiDrawText(const char* text, float x, float y, uint8_t fontSize, myColor color)
{
    
}
void OpenGLAPI::apiDrawCentredTextWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color)
{
    
}
void OpenGLAPI::apiDrawTxtWithFont(uint8_t idFont , const char *text, uint16_t posX, uint16_t posY, uint8_t fontSize, myColor color)
{
    
}
void OpenGLAPI::apiDrawImage(const std::string& src, float x, float y, float scale, myColor col)
{
    while (glGetError() != GL_NO_ERROR) { }

    initQuad();  // Asegura que el quad esté inicializado

    GLuint textureID = loadTextureCached(src);
    if (textureID == 0) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(shader2DUniforms.uImage, 0); 
    glUniform4f(shader2DUniforms.uColor,
                col.r / 255.0f, col.g / 255.0f,
                col.b / 255.0f, col.a / 255.0f);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glUniform2f(shader2DUniforms.uResolution, float(w), float(h));

    int texW = 0, texH = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &texW);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texH);

    float finalW = texW * scale;
    float finalH = texH * scale;

    glUniform2f(shader2DUniforms.uPos,  x, y);
    glUniform2f(shader2DUniforms.uSize, finalW, finalH);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

float OpenGLAPI::apiGetDeltaTime()
{
    double current = glfwGetTime();
    if (lastTime == 0.0) {
        lastTime = current;
        return 0.0f;
    }
    double delta = current - lastTime;
    lastTime = current;
    return static_cast<float>(delta);
}
float OpenGLAPI::apiGetWindowTime()
{
    return static_cast<float>(glfwGetTime());
}

//OpenGL devuelve las posiciones como doubles y nuestro struct es de floats
myVector2 OpenGLAPI::apiGetMouse()
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return myVector2{static_cast<float>(x), static_cast<float>(y)};

}
bool OpenGLAPI::apiClickMouse()
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    bool isPressed = state == GLFW_PRESS;

    bool clicked = isPressed && !wasPressedLastFrame;

    wasPressedLastFrame = isPressed;
    return clicked;
}
myInput OpenGLAPI::apiReturnInput()
{
    myInput structInput {};
    int lastInput {0};
    float RJ {0.0f};    
    float LJ {0.0f};
    float R2 {0.0f};
    float L2 {0.0f};

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {

        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            // Botones del gamepad

            
            bool currentShiftState = state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;

            // Detectar flanco de subida (solo cuando se acaba de pulsar)
            if (lastShiftState && !currentShiftState) {
                lastInput |= ComponentKey::COMPONENT_ENTER;
            }

            // Guardar estado para el siguiente frame
            lastShiftState = currentShiftState;
            

            // Bumper izquierdo y derecho
            if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS)
                lastInput |= ComponentKey::COMPONENT_SP;
            if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS)
                lastInput |= ComponentKey::COMPONENT_SP;

            // Ejes del gamepad
            float axisX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            if (axisX > 0.06f)
            {
                lastInput |= ComponentKey::COMPONENT_D;
                LJ = axisX;
            }
            if (axisX < -0.06f)
            {
                lastInput |= ComponentKey::COMPONENT_A;
                LJ = axisX;
            }

            if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.06f)
            {
                lastInput |= ComponentKey::COMPONENT_S;
                L2 = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
            }

            if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.06f)
            {
                lastInput |= ComponentKey::COMPONENT_W;
                R2 = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
            }

            RJ = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]; // eje horizontal del stick derecho
        }
    }
    else
    {
        // Teclado como fallback
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_W;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_S;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_A;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_D;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_SP;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_CTRL;
            
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            lastInput |= ComponentKey::COMPONENT_ESC;

        bool currentShiftState = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        if (currentShiftState && !lastShiftState) {
            lastInput |= ComponentKey::COMPONENT_SF;   
        }
        lastShiftState = currentShiftState;
        //if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            
        //if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
            //lastInput |= ComponentKey::COMPONENT_ENTER;
    }

    structInput.inputMask = lastInput;
    structInput.L2 = L2;
    structInput.R2 = R2;
    structInput.LJ = LJ;
    structInput.RJ = RJ;

    return structInput;
}






void OpenGLAPI::initQuad()
{
    if (quadVAO != 0) return;

    float quadVertices[] = {
        // pos.x pos.y   tex.u tex.v
        0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, 0.0f,     1.0f, 0.0f,
        1.0f, 1.0f,     1.0f, 1.0f,

        0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 1.0f,     0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(quadVertices),
                 quadVertices,
                 GL_STATIC_DRAW);

    // aPos (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    // aTex (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLAPI::loadModelCachedScreen(const std::string& path,const std::string& pathT)
{
    loadModelCached(path, pathT);
}

std::vector<meshGL> OpenGLAPI::loadModelCached(const std::string& path,
                                               const std::string& pathT)
{
   if (modelCache.find(path) != modelCache.end()) {
        return modelCache[path];
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_SortByPType |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_OptimizeMeshes |
        aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return {};
    }

    std::vector<meshGL> meshes;
    meshes.reserve(scene->mNumMeshes);

    GLuint texID = loadTextureCached(pathT);

    for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* ai_mesh = scene->mMeshes[m];

        if (!ai_mesh->HasNormals()) {
            continue;
        }

        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        vertices.reserve(ai_mesh->mNumVertices * 8);
        indices.reserve(ai_mesh->mNumFaces * 3);

        glm::vec3 minP(FLT_MAX), maxP(-FLT_MAX);

        for (unsigned i = 0; i < ai_mesh->mNumVertices; ++i) {
            const aiVector3D& v = ai_mesh->mVertices[i];
            const aiVector3D& n = ai_mesh->mNormals[i];

            glm::vec3 pos(v.x, v.y, v.z);
            minP = glm::min(minP, pos);
            maxP = glm::max(maxP, pos);

            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);

            if (ai_mesh->HasTextureCoords(0)) {
                vertices.push_back(ai_mesh->mTextureCoords[0][i].x);
                vertices.push_back(ai_mesh->mTextureCoords[0][i].y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
        }

        glm::vec3 localCenter = (minP + maxP) * 0.5f;
        float maxDist2 = 0.0f;
        for (unsigned i = 0; i < ai_mesh->mNumVertices; ++i) {
            glm::vec3 pos(vertices[i * 8], vertices[i * 8 + 1], vertices[i * 8 + 2]);
            float d2 = glm::length2(pos - localCenter);
            maxDist2 = std::max(maxDist2, d2);
        }
        float radius = std::sqrt(maxDist2);

        for (unsigned i = 0; i < ai_mesh->mNumFaces; ++i) {
            aiFace& face = ai_mesh->mFaces[i];
            if (face.mNumIndices != 3) {
                std::cerr << "[Warning] Cara con " << face.mNumIndices << " índices en " << path << ", mesh " << m << "\n";
                continue; 
            }
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                     vertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                     indices.data(),
                     GL_STATIC_DRAW);

        int stride = 8 * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);

        aiMaterial* mat = scene->mMaterials[ai_mesh->mMaterialIndex];
        aiColor3D diffCol(1.0f, 1.0f, 1.0f);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffCol);
        glm::vec3 matColor(diffCol.r, diffCol.g, diffCol.b);

        meshGL meshItem;
        meshItem.vao                  = vao;
        meshItem.vbo                  = vbo;
        meshItem.ebo                  = ebo;
        meshItem.indexCount           = static_cast<int>(indices.size());
        meshItem.textureID            = texID;
        meshItem.materialColor        = matColor;
        meshItem.boundingSphereRadius = radius;
        meshItem.localCenter          = localCenter;

        meshes.push_back(meshItem);
    }

    modelCache.emplace(path, meshes);
    return meshes;
}



meshGL OpenGLAPI::createCubeTest()
{
    float vertices[] = {
        // posiciones      uvs         normales
        -0.5f, -0.5f, -0.5f, 0.0f,0.0f, 0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,0.0f, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f,1.0f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f,0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f,1.0f, 0.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0,1,2, 2,3,0,  // detras
        4,5,6, 6,7,4,  // delante
        4,0,3, 3,7,4,  // izquierda
        1,5,6, 6,2,1,  // derecha
        4,5,1, 1,0,4,  // abajo
        3,2,6, 6,7,3   // arriba
    };

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    meshGL cubeMesh;
    cubeMesh.vao = vao;
    cubeMesh.vbo = vbo;
    cubeMesh.ebo = ebo;
    cubeMesh.indexCount = 36; 
    cubeMesh.textureID = 0;

    return cubeMesh;
}


void OpenGLAPI::extractPlane(int idx,
                             const glm::vec4& row4,
                             const glm::vec4& rowX)
{
    glm::vec4 p = row4 + rowX;
    float    len = glm::length(glm::vec3(p));
    frustumPlanes[idx] = p / len;  // (A,B,C,D) normalizado
}


bool OpenGLAPI::isSphereInsideFrustum(const glm::vec3& center,
                                      float radius) const
{
    for (auto& p : frustumPlanes) {
        if (glm::dot(glm::vec3(p), center) + p.w < -radius)
            return false;
    }
    return true;
}

void OpenGLAPI::apiPutLigth(myVector4 position, myColor color)
{
    lightGL luz = {
        glm::vec3(position.x, 22, position.y),
        glm::vec3(1.0f, 1.0f, 1.0f), // color
        0.5f, // intensidad
        25.0f // radio
    };
    
    lights.push_back(luz);
    
}

void OpenGLAPI::clearLights()
{
    lights.clear();
}

void OpenGLAPI::apiPlayVideo(const std::string& path, float w, float h, bool loop)
{
    m_videoPlayer = std::make_unique<VLCVideoPlayer>();

    m_videoPlayer->setLoop(loop); 

    int videoWidth = static_cast<int>(w);
    int videoHeight = static_cast<int>(h);

    if (!m_videoPlayer->load(path, videoWidth, videoHeight)) {
        return;
    }

    if (loop)
    {
        m_videoPlayer->play();  
    }
    

    glGenTextures(1, &m_videoTextureID);
    glBindTexture(GL_TEXTURE_2D, m_videoTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGLAPI::updateVideo()
{
    if (!m_videoPlayer || m_videoTextureID == 0) return;
    m_videoPlayer->play();  

    const uint8_t* frameData = m_videoPlayer->getFrameData();
    if (frameData) {
        glBindTexture(GL_TEXTURE_2D, m_videoTextureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            m_videoPlayer->getWidth(),
            m_videoPlayer->getHeight(),
            GL_RGB, GL_UNSIGNED_BYTE, frameData);
    }

    drawVideoFrame(0.0f, 0.0f, 1.0f); 
}

void OpenGLAPI::drawVideoFrame(float x, float y, float scale)
{
    if (!m_videoPlayer) {
        return;
    }

    if (m_videoTextureID == 0) {
        return;
    }

    const uint8_t* frameData = m_videoPlayer->getFrameData();
    if (!frameData) {
        return;
    }


    glBindTexture(GL_TEXTURE_2D, m_videoTextureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
        m_videoPlayer->getWidth(),
        m_videoPlayer->getHeight(),
        GL_RGB, GL_UNSIGNED_BYTE, frameData);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        return;
    }

    initQuad();
    if (quadVAO == 0) {
        return;
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader2D);
    if (shader2D == 0) {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_videoTextureID);
    glUniform1i(shader2DUniforms.uImage, 0);
    glUniform4f(shader2DUniforms.uColor, 1.0, 1.0, 1.0, 1.0);

    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glUniform2f(shader2DUniforms.uResolution, float(fbWidth), float(fbHeight));

    float texW = m_videoPlayer->getWidth();
    float texH = m_videoPlayer->getHeight();

    float finalW = static_cast<float>(fbWidth);
    float finalH = static_cast<float>(fbHeight);

    glUniform2f(shader2DUniforms.uPos, 0.0f, 0.0f);
    glUniform2f(shader2DUniforms.uSize, finalW, finalH);

    glBindVertexArray(quadVAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void OpenGLAPI::unloadVideo()
{
    if (m_videoPlayer) {
        m_videoPlayer->stop();
        m_videoPlayer.reset(); // Libera el recurso correctamente
    }

}