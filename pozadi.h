#ifndef POZADI_H
#define POZADI_H
#include <SDL/SDL.h>
#include <math.h>
#include "grafika.h"
#include <list>

class Pozadi
{
public:
    Pozadi();
    int u;
    int nahx[20];
    int nahy[20];
    int svitivost;
    float svitivostkometa;
    void kresli();
    void pohni();
    void zmenu();
};

#endif // POZADI_H
