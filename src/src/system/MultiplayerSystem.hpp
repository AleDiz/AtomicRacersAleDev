#pragma once

#ifndef MULTIPLAYERSYSTEM_H
#define MULTIPLAYERSYSTEM_H

#include "../util/UsingTypes.hpp"
#include "../util/OnlineStateRace.hpp"
#include <vector>

class NetEngine;
class MultiplayerSystem {
    public:
        void initConnection(NetEngine& NE);
        void waitPlayersUpdate(EManager& EM, NetEngine& NE);
        void gameUpdateIN(EManager& EM, NetEngine& NE);
        void gameUpdateOUT(EManager& EM, NetEngine& NE);
        void syncPlayersID(EManager& EM, NetEngine& NE);
        void sendToCreatePlayers(EManager& EM, NetEngine& NE);

        void initGame(NetEngine& NE);
        void disconnect(NetEngine& NE);
        OnlineStateRace const& getStateRace(NetEngine& NE) const;
        void setStateRace(NetEngine& NE, OnlineStateRace state);

};

#endif

