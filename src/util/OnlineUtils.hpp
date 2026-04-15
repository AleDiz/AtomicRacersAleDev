#pragma once

#ifndef ONLINEUTILS_H
#define ONLINEUTILS_H

#include <cstdint>
#include <string>

#include "PowerUpsTypes.hpp"
#include "BasicUtils.hpp"

struct sPlayerCreation
{
    uint32_t nUniqueID              {0};
    uint32_t nModelCarID            {0};

    const char* playerName          = "Name";
    myVector3    position           {};
    uint8_t     playerPos           {0};

};

struct sPlayerInput
{
    uint32_t nUniqueID              {0};
    int playerinput                 {0};
    myInput playerinputC            {};
    bool pendingPowerUp             {false};

    PowerUps playerPowerUp          {ANYONE};    
    int      tamPowerUp             {0}; 
};

struct sPlayerSync
{
    uint32_t nUniqueID              {0};
    int playerinput                 {0};
    myInput playerinputC            {};
    bool pendingPowerUp             {false};

    PowerUps playerPowerUp          {ANYONE};  
    int      tamPowerUp             {0};

    myVector3    position           {};
    myQuaternion rotation           {};
    myVector3    linearVelocity     {};
    myVector3    angularVelocity    {};
};


#endif