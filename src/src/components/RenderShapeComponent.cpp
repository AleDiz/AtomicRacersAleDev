#include "RenderShapeComponent.hpp"

void RenderShapeComponent::initRender3dComponent(float H, float W, float L, myColor color)
{
    shape.position = {0.0f, 0.0f, 0.0f};
    shape.rotation = {0.0f, 0.0f, 0.0f, 0.0f};
    shape.width = W;
    shape.height = H;
    shape.length = L;
    shape.color = color;

}

void RenderShapeComponent::clearComponent(){

}