#ifndef GACHASCENE_H
#define GACHASCENE_H

#include "Scene.hpp"

#include "../Entity/Entity.hpp"
#include "../man/EntityManager.hpp"
#include "../man/EntityFactory.hpp"
#include "../man/EventManager.hpp"

#include "../commons/systemCommons.h"
#include "../commons/ComponentCommons.h"

#define MAX_RAND_RARERITY 10        // Valor entre 0 y 9

enum class Rareza { COMUN, RARO, LEGENDARIO };
enum class GachaState {PRE, TIRADA, POS};

// Escena de opciones en menú principal
class GachaScene : public Scene {
private:
    EntityFactory   Factory;
    EManager        EM;
    EventManager    EVM;

    std::array<E*, 16>   buttons;
    std::array<E*, 30>   numbers;
    std::array<E*, 16>  sprites;

    E* muestra;
    E* camera;

    float timer     {0.0f};
    float timeToLoad {2.0f};
    bool initTimer  {false};

    bool tirada {true};
    bool continuar {false};

    bool recompensa {false};
    bool repetido {false};

    RenderSystem    Rsystem;
    ButtonSystem    Bsystem;
    SoundSystem     Ssystem;
    eventSystem     ESystem;

    E* musicMenu;

    GachaState estado{GachaState::PRE};
    int idObtenido {0};
public:
    // Constructor
    GachaScene(GameManager& gm) : Scene(gm) {}


    // Métodos de inicialización y actualización
    void init() override;
    void update(float deltaTime) override;
    void update_sim(float simDeltaTime) override;
    void render() override;
    void cleanup() override;

    void loadMenu(std::string path, EManager& EM, EntityFactory& EF);
    void tirar();
    void calcularMonedas();
    void comprobarTirada();
};

#endif 