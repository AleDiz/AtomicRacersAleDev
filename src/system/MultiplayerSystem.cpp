#include "MultiplayerSystem.hpp"
#include "../Entity/Entity.hpp"                // Entidades
#include "../man/EntityManager.hpp"
#include "../engine/NetEngine.hpp"              // Motor de red

void MultiplayerSystem::initConnection(NetEngine& NE)
{
    NE.initConnection();
}
void MultiplayerSystem::waitPlayersUpdate(EManager& EM, NetEngine& NE)
{
    NE.ProcessIncomingMessagesWaitingPlayers(EM);
}

void MultiplayerSystem::gameUpdateIN(EManager& EM, NetEngine& NE)
{
    NE.ProcessIncomingMessagesOnGame(EM);
}

void MultiplayerSystem::gameUpdateOUT(EManager& EM, NetEngine& NE)
{
    NE.ProcessOutgoingMessagesOnGame(EM);
}

void MultiplayerSystem::syncPlayersID(EManager& EM, NetEngine& NE)
{
    for (auto const& e : EM.getEntitiesAux1()) 
    {
        auto& multi = EM.getComponent<MultiplayerComponent>(EM.getEntityByPos(e)->getComponentKey<MultiplayerComponent>().value());

        // Funcion que guarde en el mapa de NETENGINE el uniqueID con la posicion de un coche, pasandole por parámetro estos dos mismos
        NE.syncPlayersWithID(multi.idServer, e);
    }
}

void MultiplayerSystem::sendToCreatePlayers(EManager& EM, NetEngine& NE)
{
    for (const auto& pair : NE.getPlayersOnCreation()) {
        const sPlayerCreation& playerData = pair.second;

        EM.addEntityOnlineRequest(playerData);
    }
}

void MultiplayerSystem::initGame(NetEngine& NE)
{
    NE.initGame();
}

void MultiplayerSystem::disconnect(NetEngine& NE)
{
    NE.disconnect();
}

OnlineStateRace const& MultiplayerSystem::getStateRace(NetEngine& NE) const
{
    OnlineStateRace stateRace = NE.getStateRace();
    return stateRace;
}

void MultiplayerSystem::setStateRace(NetEngine& NE, OnlineStateRace state)
{
    NE.setStateRace(state);
}
