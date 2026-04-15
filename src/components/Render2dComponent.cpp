#include "Render2dComponent.hpp"

void Render2dComponent::initRender2dComponent(int givenID, int givenX, int givenY, int givenAlpha){
    id = givenID;
    position.x = givenX;
    position.y = givenY;
    
    float floatAlpha = givenAlpha;
    color = myColor{255,255,255,floatAlpha};
}

void Render2dComponent::clearComponent(){

}