#include "pozadi.h"
#include <SDL/SDL.h>
#include <math.h>
#include "grafika.h"
#include <list>


Pozadi::Pozadi()
//urcim pozice hvezdicek
{
	for (int i = 0;20 > i;i++)
	{
		nahx[i] = nahoda(799);
		nahy[i] = nahoda(300);
	}
	u = 1;
}



void Pozadi::pohni()
//hybu s kometou na pozadi
{
	for(int i =0; i<20; i++)
	{
		nahy[i]++;
		if (nahy[i]==600){
			nahy[i]=0;

		}
	}
}

void Pozadi::kresli()

{
    srand(8);
    //maluju pozadi nocni oblohy
	if (u == 1){
		for(int i = 1;i < 600;i++){
			barva(0,0,i/8);
			cara(0,i,799,i);
		}

		for(int o =1; o< 1500;o++){
			svitivost = nahoda(255);
			barva(svitivost,svitivost,svitivost);
			bod(nahoda(800),nahoda(600));
		}

		for(int i =0 ;i<20;i++){
			svitivostkometa = nahoda(1);
			barva(160*svitivostkometa, 0, 40*svitivostkometa);
			kruh(nahx[i],nahy[i],nahoda(5));
		}}
	// maluji zapad slunce
	else{
		for(int i = 0;i < 600;i++){
			barva(255,180-i/4,0);
			cara(0,i,799,i);
		}

		barva(255, 250, 0);
		barva(255, 130, 0);
		kruh(400,300,250);
		barva(255, 150, 0);
		kruh(400,300,230);
		barva(255, 170, 0);
		kruh(400,300,200);
		barva(255, 190, 0);
		kruh(400,300,170);



	}
}
void Pozadi::zmenu(){
//prikaz jenz meni pozadi

	u++;
	if ( u == 3)
		u = 1;

}
