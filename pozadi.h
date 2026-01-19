#ifndef POZADI_H
#define POZADI_H
#include <SDL/SDL.h>
#include <math.h>
#include "grafika.h"
#include <list>

#define POZADI_H

#include <SDL/SDL.h>

enum FazeDne {
    DEN,
    NOC,
    VYCHOD,
    ZAPAD
};

class Pozadi {
public:
    Pozadi();

    void pohni();

    void kresli();

    void aktualizuj();

private:
    float nahx[50];
    float nahy[50];

    float uhelSlunce;

    int casovac;

    FazeDne faze;

    float svitivost;
    float svitivostkometa;
};


#endif // POZADI_H
