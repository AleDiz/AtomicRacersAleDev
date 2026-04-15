#pragma once

#ifndef NETENGINE_H
#define NETENGINE_H

#include "../net/AM_common.h"
#include "../util/UsingTypes.hpp"
#include "../util/OnlineStateRace.hpp"

#include <map>


class NetEngine : public net::client_interface<GameMsg>
{
    private:
        sPlayerSync         tempPlayerSync{};       // Sincronizacion de un jugador (posicion, rotacion, velocidad...)
        sPlayerCreation     tempPlayerCreation{};   // Creacion de un jugador
        sPlayerInput        tempPlayerInput{};      // Input de un jugador

        uint32_t nPlayerID = 0;
        uint32_t nPlayers = 0;

        // Almacenamos el ID que nos corresponde en el servidor junto con la posición que corresponde en el array de entidades
        std::map<uint32_t, int> conversionID;

        // Almacenamos los jugadores que se han conectado al servidor junto con su información
        std::map<uint32_t, sPlayerCreation> playersOnCreation;

        // Estado de la carrera
        OnlineStateRace stateRace {WAITING};

    std::chrono::steady_clock::time_point lastUpdatePlayer = std::chrono::steady_clock::now();


    public:
        void initConnection();
        void ProcessIncomingMessagesWaitingPlayers(EManager& EM);
        void ProcessIncomingMessagesOnGame(EManager& EM);
        void ProcessOutgoingMessagesOnGame(EManager& EM);

        void syncPlayersWithID(uint32_t ID, int posEntity);

        void initGame();

        void disconnect();
        OnlineStateRace const& getStateRace() const;
        uint32_t const& getnPlayerID() const;
        std::map<uint32_t, sPlayerCreation> const& getPlayersOnCreation() const;

        void setStateRace(OnlineStateRace state);
};


#endif