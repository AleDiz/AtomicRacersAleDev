#ifndef BUTTONCOMPONENT_H
#define BUTTONCOMPONENT_H

#include <cstdint>

class ButtonComponent {
public:
    int posX;
    int posY;
    int sizeX;
    int sizeY;
    int id;

    bool hover;

    static constexpr int Capacity{100};

    void initButtonComponent(int givenId,int givenX, int givenY, int givenSX, int givenSY);
    void clearComponent();
};

#endif