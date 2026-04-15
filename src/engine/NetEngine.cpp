#include "NetEngine.hpp"
#include "../man/EntityManager.hpp"
#include "../Entity/Entity.hpp"
#include "../util/JsonManager.hpp"

#include <chrono>

void NetEngine::initConnection()
{
    // Iniciar la conexión con el servidor
    if (!Connect("138.68.140.196", 12345))
    {
        std::cerr << "Error: No se pudo conectar al servidor.\n";
    }
}

void NetEngine::ProcessIncomingMessagesWaitingPlayers(EManager& EM)
{
    if (!Incoming().empty())
    {
        auto msg = Incoming().pop_front().msg;

        switch (msg.header.id)
        {
            // Mensajes del servidor - SIN USO
            case GameMsg::Server_Accept:
            {
                std::cout << "Servidor aceptado\n";
                break;
            }

            // Mensajes del cliente
            case GameMsg::Client_Validated:
            {
                std::cout << "Cliente validado\n";
                std::cout << "Comenzando registro\n";
                break;
            }

            case GameMsg::Client_Accepted:
            {
                // Crear y enviar al servidor
                // Enviar estructura para crear el jugador actual en el servidor con la informacion del usuario
                // Aqui hay que rellenar el struct tempPlayerCreation con la informacion del jugador
                net::message<GameMsg> msg;
                msg.header.id = GameMsg::Client_RegisterWithServer;
                
                JSONManager Json;
                Json.loadJSON("assets/JSON/playerConfig.json");
                tempPlayerCreation.nModelCarID = Json.getInt("PlayerCar");
                tempPlayerCreation.playerName = "Player";                      // TO DO: Cambiar por el nombre del jugador

                msg << tempPlayerCreation;

                Send(msg);

                std::cout << "Enviando mensaje de registro al servidor\n";
                break;
            }

            case GameMsg::Client_AssingID:
            {
                // Almacenamos el ID que nos corresponde en el servidor
                msg >> nPlayerID;
                break;
            }

            // Mensajes del juego
            case GameMsg::Game_AddPlayer:
            {
                JSONManager Json;
                Json.loadJSON("assets/JSON/circuits/salonjuguetes.json");

                // Añadimos un jugador nuevo al mundo gracias al struct tempPlayerCreation
                msg >> tempPlayerCreation;
                std::cout << "Añadiendo jugador con ID: " << tempPlayerCreation.nUniqueID << "\n";


                tempPlayerCreation.position = Json.getVector3(std::to_string(tempPlayerCreation.playerPos));
                playersOnCreation.insert_or_assign(tempPlayerCreation.nUniqueID, tempPlayerCreation);
                
                nPlayers++;
                break;
            }

            case GameMsg::Game_StartGame:
            {
                // Iniciar la partida
                std::cout << "Iniciando partida\n";
                stateRace = OnlineStateRace::GAME;
                
                break;
            }

            case GameMsg::Game_StartCreation:
            {
                // Iniciar la creacion de la partida
                std::cout << "Iniciando creacion de la partida\n";
                stateRace = OnlineStateRace::CHANGETOGAME;
                break;
            }
        }
    }
}

void NetEngine::ProcessIncomingMessagesOnGame(EManager& EM)
{
    if (!Incoming().empty())
    {
        auto msg = Incoming().pop_front().msg;

        switch (msg.header.id)
        {
            case GameMsg::Game_UpdatePlayer_Input:
            {
                for (int i = 0; i < nPlayers; i++)
                {
                    tempPlayerInput = {};
                    msg >> tempPlayerInput;

                    // Si no somos el propio jugador, actualizar el estado del jugador
                    if (tempPlayerInput.nUniqueID != nPlayerID)
                    {
                        int entityPosition = conversionID[tempPlayerInput.nUniqueID];

                        if(EM.hasComponent<MultiplayerComponent>(*EM.getEntityByPos(entityPosition))){
                            auto& vK = EM.getComponent<MultiplayerComponent>(EM.getEntityByPos(entityPosition)->getComponentKey<MultiplayerComponent>().value());
                            vK.inputServer.inputMask = tempPlayerInput.playerinputC.inputMask;
                            vK.inputServer.L2 = tempPlayerInput.playerinputC.L2;
                            vK.inputServer.LJ = tempPlayerInput.playerinputC.LJ;
                            vK.inputServer.R2 = tempPlayerInput.playerinputC.R2;
                            vK.inputServer.RJ = tempPlayerInput.playerinputC.RJ;
                            
                            vK.poweUpServer = tempPlayerInput.playerPowerUp;
                            vK.tamPowerUp = tempPlayerInput.tamPowerUp;
                        }

                        // REVISAR QUEMAS COSAS TENEMOS QUE PASAR DE POWER UP
                    }
                }

                break;
            }

            case GameMsg::Game_UpdatePlayer_Sync:
            {
                for (int i = 0; i < nPlayers; i++)
                {
                    tempPlayerSync = {};
                    msg >> tempPlayerSync;

                    // Si no somos el propio jugador, actualizar el estado del jugador
                    if (tempPlayerSync.nUniqueID != nPlayerID)
                    {   

                        int entityPosition = conversionID[tempPlayerSync.nUniqueID];
                        if(EM.hasComponent<MultiplayerComponent>(*EM.getEntityByPos(entityPosition))){
                            auto& vK = EM.getComponent<MultiplayerComponent>(EM.getEntityByPos(entityPosition)->getComponentKey<MultiplayerComponent>().value());
                            vK.inputServer.inputMask = tempPlayerSync.playerinputC.inputMask;
                            vK.inputServer.L2 = tempPlayerSync.playerinputC.L2;
                            vK.inputServer.LJ = tempPlayerSync.playerinputC.LJ;
                            vK.inputServer.R2 = tempPlayerSync.playerinputC.R2;
                            vK.inputServer.RJ = tempPlayerSync.playerinputC.RJ;
                            vK.poweUpServer = tempPlayerSync.playerPowerUp;
                            vK.tamPowerUp = tempPlayerSync.tamPowerUp;

                            vK.dataSaves = tempPlayerSync;

                            vK.checkSync = true;
                        }
                    }
                }

                break;
            }

            case GameMsg::Game_RemovePlayer:
            {
                uint32_t nID;
                msg >> nID;
                // Mediante el id para borrar el jugador, deberiamos quitarlo de la lista de entidades

                EM.getEntityByPos(conversionID[nID])-> state = false;

                nPlayers--;
                break;
            }
        }
    }
}

void NetEngine::ProcessOutgoingMessagesOnGame(EManager& EM)
{

    net::message<GameMsg> msg;
    msg.header.id = GameMsg::Game_UpdatePlayer;
    // TO DO: Rellenar la estructura tempPlayerCreation con la informacion del jugador

    auto& vc = EM.getComponent<VehicleComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<VehicleComponent>().value());

    myVector3 position 
    {
        vc.m_carChassis->getWorldTransform().getOrigin().getX(),
        vc.m_carChassis->getWorldTransform().getOrigin().getY(),
        vc.m_carChassis->getWorldTransform().getOrigin().getZ()
    };

    myVector3 lVel 
    {
        vc.m_carChassis->getLinearVelocity().getX(),
        vc.m_carChassis->getLinearVelocity().getY(),
        vc.m_carChassis->getLinearVelocity().getZ()
    };

    myVector3 aVel 
    {
        vc.m_carChassis->getAngularVelocity().getX(),
        vc.m_carChassis->getAngularVelocity().getY(),
        vc.m_carChassis->getAngularVelocity().getZ()
    };

    myQuaternion rotation 
    {
        vc.m_carChassis->getWorldTransform().getRotation().getW(),
        vc.m_carChassis->getWorldTransform().getRotation().getX(),
        vc.m_carChassis->getWorldTransform().getRotation().getY(),
        vc.m_carChassis->getWorldTransform().getRotation().getZ()
    };

    tempPlayerSync.position         = position;
    tempPlayerSync.rotation         = rotation;
    tempPlayerSync.linearVelocity   = lVel;
    tempPlayerSync.angularVelocity  = aVel;
    tempPlayerSync.nUniqueID        = nPlayerID;

    if(EM.hasComponent<InputComponent>(*EM.getEntityByType(EntityType::PLAYER))){
        auto& ic = EM.getComponent<InputComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<InputComponent>().value());
        tempPlayerSync.playerinput      = ic.actionInput.inputMask;

        tempPlayerSync.playerinputC.inputMask   = ic.actionInput.inputMask;   
        tempPlayerSync.playerinputC.L2          = ic.actionInput.L2;
        tempPlayerSync.playerinputC.LJ          = ic.actionInput.LJ;
        tempPlayerSync.playerinputC.R2          = ic.actionInput.R2;
        tempPlayerSync.playerinputC.RJ          = ic.actionInput.RJ;
        

    }else if(EM.hasComponent<AIComponent>(*EM.getEntityByType(EntityType::PLAYER))){
        auto& ai = EM.getComponent<AIComponent>(EM.getEntityByType(EntityType::PLAYER)->getComponentKey<AIComponent>().value());
        tempPlayerSync.playerinputC.inputMask   = ai.actionInput.inputMask;   
        tempPlayerSync.playerinputC.L2          = ai.actionInput.L2;
        tempPlayerSync.playerinputC.LJ          = ai.actionInput.LJ;
        tempPlayerSync.playerinputC.R2          = ai.actionInput.R2;
        tempPlayerSync.playerinputC.RJ          = ai.actionInput.RJ;
    }

    tempPlayerSync.playerPowerUp            = vc.powerUp;
    tempPlayerSync.tamPowerUp               = vc.tamPowerUp;


    msg << tempPlayerSync;
    Send(msg);
}

void NetEngine::syncPlayersWithID(uint32_t ID, int posEntity)
{
    conversionID.insert_or_assign(ID, posEntity);
}

void NetEngine::initGame()
{
    net::message<GameMsg> msg;
    msg.header.id = GameMsg::Game_Init;
    Send(msg);
}

void NetEngine::disconnect()
{
    this->m_connection.get()->Disconnect();

    // Limpiar los datos
    nPlayerID = 0;
    nPlayers = 0;
    playersOnCreation.clear();
    conversionID.clear();
    stateRace = OnlineStateRace::WAITING;

    // Limpiar los mensajes
    while (!Incoming().empty())
    {
        Incoming().pop_front();
    }

}
OnlineStateRace const& NetEngine::getStateRace() const 
{
    return stateRace;
}

uint32_t const& NetEngine::getnPlayerID() const 
{
    return nPlayerID;
}
std::map<uint32_t, sPlayerCreation> const& NetEngine::getPlayersOnCreation() const
{
    return playersOnCreation;
}

void NetEngine::setStateRace(OnlineStateRace state)
{
    stateRace = state;
}