#pragma once

#ifndef BUTTONSYSTEM_H
#define BUTTONSYSTEM_H

#include "../util/UsingTypes.hpp"
class GameManager;
class EventManager;

class ButtonSystem {
private:

public:
    static void update(EManager& EM, GameManager& GM, EventManager& EVM, int WW, int WH, int MX, int MY, bool clicked);
    static void update_one_entity_button(E& e, GameManager& GM, EventManager& EVM, int WW, int WH, int MX, int MY, bool clicked);
};

#endif