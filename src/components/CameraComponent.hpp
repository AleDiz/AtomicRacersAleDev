#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "../util/BasicUtils.hpp"

#include <chrono>

constexpr float HEIGHTABOVECAMERA       {3.0f};       //Altura de la camara por encima del player
constexpr float DISTANCECAMERA          {8.0f};       //Distancia de la camara detrás del player
constexpr float FOVCAMERA               {80.0f};      //Fov de la camara de detrás del player

enum CameraType{
    FOLLOW,
    ORBITAL
};

class CameraComponent {
public:
    myVector3 pos           {0, 0, 0}; 
    myVector3 target        {0, 0, 0};
    myVector3 up            {0, 1, 0};
    float fovy              {FOVCAMERA};
    int projection          {0};
    float smoothedDistance  {DISTANCECAMERA};
    float smoothedFov       {FOVCAMERA};
    CameraType camType      {FOLLOW};

    uint8_t cinematicIt {4};
    bool cinematicRestart {false};
    std::chrono::_V2::system_clock::time_point cinematicStartTime {};
    std::array <myVector3, 12> cinematicPos {};

    void clearComponent(){
            
    };

    static constexpr int Capacity{4};

};

#endif // CAMERA_COMPONENT_H