#include "man/GameManager.hpp"
#include "util/RenderTypes.hpp"
#include "util/JsonManager.hpp"

// constexpr int WIDTH = 1280;
// constexpr int HEIGHT = 720;
constexpr char WINDOW_NAME[] = "Atomic Racers";
constexpr int FPS = 60;

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    int WIDTH = 1280;
    int HEIGHT = 720;
    RenderType renderType = RenderType::Raylib;

    JSONManager jsonManager;
    jsonManager.loadJSON("assets/JSON/player/state.json");
    int resolucion = jsonManager.getInt("resolucion");
    if (resolucion)
    {
        WIDTH = 1920;
        HEIGHT = 1080;
    }
    int modoPantalla = jsonManager.getInt("modoPantalla");
    int motor = jsonManager.getInt("motor");
    if (motor)
    {
        renderType = RenderType::OpenGL;
    }

    GameManager gameManager = GameManager(WIDTH, HEIGHT, WINDOW_NAME, FPS, renderType);

    gameManager.run();

    return 0;
}