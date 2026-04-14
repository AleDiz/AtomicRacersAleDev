#include "ModelOwn.hpp"

void ModelOwn::loadModel3D(const char *fileName){
    modelo = LoadModel(fileName);
}

void ModelOwn::unloadModel(){
    UnloadModel(modelo);
}

void ModelOwn::drawModel(Vector3 position, float scale, Color color){
    DrawModel(modelo, position, scale, color);
}

void ModelOwn::drawRoad()
{
    drawModel({ 0.0f, 0.0f, 0.0f }, 1.0f, BLACK);
    // // PosiciÃ³n inicial del centro de la carretera
    // Vector3 roadCenter = {0.0f, 0.0f, 0.0f};
    // Vector2 size = {30.0f, 200.0f};

    // // PRIMERA CARRETERA -----------------------------
    // // Dibujar la carretera como un plano grande
    // DrawPlane(roadCenter, size, BLACK);

    // // Dibujar lineas del centro
    // for (float z = -100.0f; z < 100.0f; z += 7.0f)
    // {
    //     Vector3 start = {0.0f, 0.01f, z};
    //     Vector3 end = {0.0f, 0.01f, z + 2.0f};
    //     DrawLine3D(start, end, WHITE);
    // }
    // // -----------------------------------------------

    // // SEGUNDA CARRETERA -----------------------------
    // roadCenter = {-85.0f, 0.0f, 115.0f};
    // size = {200.0f, 30.0f};
    // // Dibujar la carretera como un plano grande
    // DrawPlane(roadCenter, size, BLACK);

    // // Dibujar lineas del centro
    // for (float x = 15.0f; x > -185.0f; x -= 7.0f)
    // {
    //     Vector3 start = {x, 0.01f, 115.0f};
    //     Vector3 end = {x - 2.0f, 0.01f, 115.0f};
    //     DrawLine3D(start, end, WHITE);
    // }
    // // -----------------------------------------------

    // // TERCERA CARRETERA -----------------------------
    // roadCenter = {-170.0f, 0.0f, 0.0f};
    // size = {30.0f, 200.0f};

    // // Dibujar la carretera como un plano grande
    // DrawPlane(roadCenter, size, BLACK);

    // // Dibujar lineas del centro
    // for (float z = -100.0f; z < 100.0f; z += 7.0f)
    // {
    //     Vector3 start = {-170.0f, 0.01f, z};
    //     Vector3 end = {-170.0f, 0.01f, z + 2.0f};
    //     DrawLine3D(start, end, WHITE);
    // }
    // // -----------------------------------------------

    // // TERCERA CARRETERA -----------------------------
    // roadCenter = {-85.0f, 0.0f, -115.0f};
    // size = {200.0f, 30.0f};

    // // Dibujar la carretera como un plano grande
    // DrawPlane(roadCenter, size, BLACK);

    // // Dibujar lineas del centro
    // for (float x = -185.0f; x < 15.0f; x += 7.0f)
    // { // Ajusta el rango para cubrir toda la carretera
    //     Vector3 start = {x, 0.01f, -115.0f};
    //     Vector3 end = {x + 2.0f, 0.01f, -115.0f};
    //     DrawLine3D(start, end, WHITE);
    // }
    // // -----------------------------------------------

    // // Dibujar Muros ----------------------------------
    // // DrawCubeV({+15.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 260.0f}, DARKBLUE);
    // // DrawCubeV({-15.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 200.0f}, DARKBLUE);

    // // Dibujar escenarios ----------------------------
    // DrawCube({-22, 25, 0}, 10, 50, 10, ORANGE);
    // DrawCubeWires({-22, 25, 0}, 10, 50, 10, BLACK);

    // DrawCube({-30, 25, 30}, 10, 50, 10, PINK);
    // DrawCubeWires({-30, 25, 30}, 10, 50, 10, BLACK);

    // DrawCube({30, 25, -45}, 10, 50, 10, GREEN);
    // DrawCubeWires({30, 25, -45}, 10, 50, 10, BLACK);

    // DrawCube({-22, 25, 45}, 10, 50, 10, PURPLE);
    // DrawCubeWires({-22, 25, 45}, 10, 50, 10, BLACK);
}

Model ModelOwn::getModel(){
    return modelo;
}