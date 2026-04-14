#pragma once
#include <raylib.h>
#include <cstdint>
#include "net.h"

#include "../util/OnlineUtils.hpp"

enum class GameMsg : uint32_t
{
    Server_Accept,                  // El servidor acepta la conexion del cliente
    Server_GetStatus,               // El cliente pide el estado del servidor
    Server_GetPing,                 // El cliente pide el ping del servidor 
    Server_GetPlayers,              // El cliente pide la lista de jugadores del servidor

    Client_Validated,               // El servidor valida al cliente
    Client_Accepted,                // El servidor acepta la conexion del cliente
    Client_AssingID,                // El servidor asigna un ID al cliente
    Client_RegisterWithServer,      // El cliente se registra en el servidor
    Client_UnregisterWithServer,    // El cliente se desregistra del servidor

    Game_Init,                      // Inicializacion de la partida
    Game_StartCreation,             // El servidor inicia la creacion de la partida
    Game_StartGame,                 // El servidor inicia la partida
    Game_AddPlayer,                 // El servidor añade un jugador
    Game_UpdatePlayer,              // El cliente actualiza un jugador
    Game_UpdatePlayer_Input,        // El servidor actualiza la entrada de un jugador
    Game_UpdatePlayer_Sync,         // El servidor actualiza la sincronizacion de un jugador (posicion, rotacion, velocidad...)
    Game_RemovePlayer               // El servidor elimina un jugador

};