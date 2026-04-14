#include "ButtonComponent.hpp"

void ButtonComponent::initButtonComponent(int givenId, int givenX, int givenY, int givenSX, int givenSY)
{
    id = givenId;
    posX = givenX;
    posY = givenY;
    sizeX = givenSX;
    sizeY = givenSY;
    hover = false;
}

void ButtonComponent::clearComponent(){
    
}