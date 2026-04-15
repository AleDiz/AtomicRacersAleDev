#include "NotAIComponent.hpp"
#include "../util/EnumsActions.hpp"

void NotAIComponent::initNotAIComponent(myVector3 p1, myVector3 p2, myQuaternion r)
{
    pos1 = p1;
    pos2 = p2;
    rot  = r;
    posActual  = pos1;
}

void NotAIComponent::clearComponent()
{

}