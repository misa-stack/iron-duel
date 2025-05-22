#include "menu.h"
#include "strela.h"
#include <math.h>
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "tank.h"
#include "raketa.h"
#include "vybuch.h"
#include "hopik.h"
void kometa(int x, int y,int r)
{
	kruh(x,y,r);
}
int main(int argc, char** argv)
{
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);

	int rychlax = 100; //nastavuji zakladni pozici strel
	int rychlay = 200; //nastavuji zakladni pozici strel
	int basex = 100; //nastavuji zakladni pozici strel
	int basey = 200; //nastavuji zakladni pozici strel
	vybuch *v = NULL;
	Krajina k;
	Pozadi pozadi;
	Obrazek zbranraketa;
	Obrazek zbrankanon;
	Obrazek momentalnizbran;
	Menu hlavni;
	int tah=0;
	int naboj=1;

	Tank tank[2];

	strela*s=NULL;
	enum Stav {
		nic,
		//	zamerovani,
		//	strelba,
		//	vybuchovani,
		padani,
	};
	enum Stav stav = nic;
	while(1)
	{
		//t1 = SDL_GetTicks();
		obrazovka->smaz();

		/* zacatek kresleni */
		//srand(1258);

		int a=tah;

		tank[1].umisti(130, k.kdeJeHlina(130));
		tank[0].umisti(670, k.kdeJeHlina(670));


		// if( k.jeHlina(rychla.x,rychla.y) == 1){
		// 	v.kresli();
		// 	if (v.rk > 100)
		// 	{
		// 		v.rk = 0;
		// 	}
		// }




		//t1 = SDL_GetTicks();
		obrazovka->smaz();


		pozadi.pohni();
		pozadi.kresli();
		k.kresli();
		//maluji popis jakou strelu pouzivam aby uzivatel vedel jakou momentalne odpaluje
		momentalnizbran.nacti("momentalnizbran.png");
		momentalnizbran.umisti(600,0);
		momentalnizbran.kresli();

		//kresleni vybuchu a padani krajiny
		if(v)
		{
			if(v->kresli(k) == true)
				stav = padani;
		}
		if(stav == padani)
			if(k.padej())
				stav = nic;




		if(s)
		{
			s->kresli();
			if(s->pohni(&k))
			{
				v = new vybuch(s->x,s->y);
				s = NULL;
			}
		}


		tank[1].kresli();
		tank[0].kresli();
		tank[a].naloz();




		hlavni.kresli();
		if (naboj==1)
		{
			barva(100,255,118);
			kruh(750,20,5);
		}
		if (naboj==2)
		{
			barva(100,255,118);
			trojuhelnik(740,15,745,30,735,30);
		}

		/* konec kresleni */
		obrazovka->aktualizuj();

		Uint8* key = SDL_GetKeyState(NULL);
		if(key[SDLK_LEFT])
		{
			tank[a].vlevo();
		}
		if(key[SDLK_RIGHT])
		{
			tank[a].vpravo();
		}
		if(key[SDLK_DOWN])
		{
			if(tank[a].prach<0.1)
			{
				tank[a].prach=0;
			}
			else
				tank[a].prach-=0.3;
		}
		if(key[SDLK_UP])
		{
			if(tank[a].prach>40)
			{
				tank[a].prach=40;
			}
			else
				tank[a].prach+=0.3;
		}

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
				case SDLK_1:
				case SDLK_KP1:
					naboj=1;
					break;
				case SDLK_2:
				case SDLK_KP2:
					naboj=2;
					break;
				case SDLK_3:
				case SDLK_KP3:
					naboj=3;
					break;
				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
				case SDLK_SPACE:
					if (s == NULL)
					{
						if (naboj==1)
						{
							s = new strela(tank[a].x+20*2*cos(tank[a].uhel), tank[a].y+20*2*sin(tank[a].uhel));
						}
						if (naboj==2)
						{
							s = new raketa(tank[a].x+20*2*cos(tank[a].uhel), tank[a].y+20*2*sin(tank[a].uhel));
						}
						if(naboj==3)
						{
							s = new hopik(tank[a].x+20*2*cos(tank[a].uhel), tank[a].y+20*2*sin(tank[a].uhel));
						}

						s->vx=tank[a].prach/3*cos(tank[a].uhel);
						s->vy=tank[a].prach/3*sin(tank[a].uhel);

						if (tah==1)
						{
							tah--;
						}
						else
							tah++;
						break;
					}
				}
				break;
			}
			/* konec kresleni */
			obrazovka->aktualizuj();

		}

	}

}
