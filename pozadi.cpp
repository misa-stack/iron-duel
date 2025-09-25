#include "pozadi.h"
#include <SDL/SDL.h>
#include <math.h>
#include "grafika.h"



Pozadi::Pozadi() {
    for (int i = 0; i < 20; i++) {
        nahx[i] = nahoda(799);
        nahy[i] = nahoda(300);
    }
    uhelSlunce = 0.0f;
    casovac = 0;
    faze = DEN;
}

void Pozadi::pohni() {
    for (int i = 0; i < 20; i++) {
        nahy[i]++;
        if (nahy[i] >= 600) {
            nahy[i] = 0;
        }
    }
}

void Pozadi::aktualizuj() {
    uhelSlunce += 0.3f;
    if (uhelSlunce >= 0.0f && uhelSlunce <= 30.0f ){
        faze = VYCHOD;}
    if (uhelSlunce > 30.0f && uhelSlunce <= 150.0f){
        faze = DEN;}
    if (uhelSlunce > 150.0f && uhelSlunce <= 180.0f){
        faze = ZAPAD;}
    if (uhelSlunce > 180.0f && uhelSlunce <=360.0f){
        faze = NOC;}
    if (uhelSlunce > 360.0f){
        uhelSlunce = 0.0f;
    }

}



void Pozadi::kresli() {
    srand(8);
    int cx = 400, cy = 300, r = 350;
    int x = cx + r * cos(uhelSlunce * M_PI / 180.0);
    int y = cy - r * sin(uhelSlunce * M_PI / 180.0);

    switch (faze) {
    case DEN:
        barva(135, 206, 250); // světle modrá
        obdelnik(0, 0, 800, 600);
        barva(255, 255, 0); // slunce
        kruh(x, y, 130);
        break;


    case NOC:
        barva(10, 10, 30);
        obdelnik(0, 0, 800, 600);
        for (int o = 0; o < 1500; o++) {
            svitivost = nahoda(255);
            barva(svitivost, svitivost, svitivost);
            bod(nahoda(800), nahoda(600));
        }
        for (int i = 0; i < 20; i++) {
            svitivostkometa = nahoda(1);
            barva(160 * svitivostkometa, 0, 40 * svitivostkometa);
            kruh(nahx[i], nahy[i], nahoda(5));
        }
        barva(255, 255, 255); // měsíc
        kruh(x, y, 130);
        break;

    case ZAPAD:
        for(int i = 0;i < 600;i++){
            barva(255,180-i/4,0);
            cara(0,i,799,i);}
        barva(255, 130, 0);
        kruh(x,y,130);
        barva(255, 150, 0);
        kruh(x,y,110);
        barva(255, 170, 0);
        kruh(x,y,90);
        barva(255, 190, 0);
        kruh(x,y,70);

    //case VYCHOD:



    }
}
