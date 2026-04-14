#pragma once

#ifndef MULTIPLAYERCOMPONENT_H
#define MULTIPLAYERCOMPONENT_H

#include <memory>
#include <vector>
#include <cstdint>

#include "../util/OnlineUtils.hpp"

class MultiplayerComponent {
public:
    static constexpr int Capacity{10};

    uint32_t idServer{0};

    bool checkSync{false};

    // DATOS QUE RECIBE DEPENDIENDO DE LA LLAMADA
    myInput inputServer{};

    sPlayerSync dataSaves{};
    sPlayerSync previousdataSaves{};

    PowerUps poweUpServer{ANYONE};
    int tamPowerUp{0};  

    double previousTimestamp {0.0};  
    double lastUpdateTimestamp {0.0};

    void clearComponent();
    void initComponent(uint32_t idServer);
    
};


#endif