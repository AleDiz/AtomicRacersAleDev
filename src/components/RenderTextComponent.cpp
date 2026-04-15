#include "RenderTextComponent.hpp"

void RenderTextComponent::initRenderTextComponent(std::string givenText, int givenX, int givenY, int givenSize, myColor givenColor)
{
    text.color = givenColor;
    text.pos.x = givenX;
    text.pos.y = givenY;
    text.size = givenSize;
    text.texto = givenText;
}

void RenderTextComponent::clearComponent(){

}