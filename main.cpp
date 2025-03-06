#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include <time.h>
#include "raketa.h"
#include "vybuch.h"

int main(int argc, char** argv)
{
	srand(time(NULL));

	int rychlax = 100; //nastavuji zakladni pozici strel
	int rychlay = 200; //nastavuji zakladni pozici strel
	int basex = 100; //nastavuji zakladni pozici strel
	int basey = 200; //nastavuji zakladni pozici strel
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);
	strela base(basex,basey); //sestavuji veci podle trid
	vybuch v(100,200);
	Krajina k;
	Pozadi pozadi;

	//int t2;
	//int t1;
	bool bas = false; //urcuji zda byla strela odpalena ci ne
	bool rychl = false; //urcuji zda byla strela odpalena ci ne
	int jakoustreluodpalujes =1;


	//int t2;
	//int t1;
	while(1)
	{
		/* zacatek kresleni */
		//srand(1258);
		pozadi.pohni(); // maluji pozadi a krajinu
		pozadi.kresli();
		base.kresli();
		base.pohni();
		k.kresli();





		Krajina k;
		Pozadi pozadi;
		raketa rychla(rychlax,rychlay);
		//int t2;
		//int t1;


		Obrazek zbranraketa;
		Obrazek zbrankanon;
		Obrazek momentalnizbran;

		if( k.jeHlina(rychla.x,rychla.y) == 1){
			v.kresli();
			if (v.rk > 100)
			{
				v.rk = 0;
			}
		}




		//t1 = SDL_GetTicks();
		obrazovka->smaz();



		pozadi.pohni();
		pozadi.kresli();
		k.kresli();
		//nastavuji recyklaci strel ze pokud x a y te strely je mimo tu obrazovku tak se jeji parametry resetujou na puvodni
		if(bas ==true){
			base.kresli();
			base.pohni();
			if (base.x > 800 || base.y >600){
				bas = false;
				base.x = basex;
				base.y = basey;
				base.vy = -3;
				base.vx = 10;
			}

		}
		if(rychl == true){
			rychla.kresli();
			rychla.pohni();
			if (rychla.x > 800 || rychla.y > 600){
				rychl = false;
				rychla.x = rychlax;
				rychla.y = rychlay;
				rychla.vy = -3;
				rychla.vx = 4;
			}

		}
		//maluji popis jakou strelu pouzivam aby uzivatel vedel jakou momentalne odpaluje
		momentalnizbran.nacti("momentalnizbran.png");
		momentalnizbran.umisti(600,0);
		momentalnizbran.kresli();
		if(jakoustreluodpalujes == 1)
		{
			zbrankanon.nacti("kanon.png");
			zbrankanon.umisti(700,10);
			zbrankanon.kresli();
		}
		if(jakoustreluodpalujes == 2)
		{
			zbranraketa.nacti("raketa.png");
			zbranraketa.umisti(700,10);
			zbranraketa.kresli();
		}

<<<<<<< HEAD
    v.kresli();


        /* konec kresleni */
        obrazovka->aktualizuj();
        SDL_Delay(16);
v.kresli();
=======
>>>>>>> 93ef8eba25b3f6b1fb8715b69281d1d29b231c71




		/* konec kresleni */
		obrazovka->aktualizuj();


		SDL_Delay(16);

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_p:
					pozadi.zmenu();
					break;
				case SDLK_k:
					k.zmena();
					break;
				case SDLK_UP:
					jakoustreluodpalujes++;
					if(jakoustreluodpalujes == 3){
						jakoustreluodpalujes= 1;
					}
					break;
				case SDLK_f:
					if(jakoustreluodpalujes==1){

						bas = true;
						break;
					}
					if(jakoustreluodpalujes==2){
						rychl = true;
						break;
					}
				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
				}
				break;
			}
		}
		//t2 = SDL_GetTicks();
		//int dt = t2 - t1;
		//if (5 - dt > 0)SDL_Delay(5 -dt);
	}
}
