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
#include "bomba.h"

void kometa(int x, int y,int r)
{
	kruh(x,y,r);
}

std::list<Tank*> tanky;
std::list<strela*> strely;
std::list<strela*>::iterator s;
std::list<vybuch*> vybuchy;
std::list<vybuch*>::iterator v;

int main(int argc, char** argv)
{
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);

	int rychlax = 100; //nastavuji zakladni pozici strel
	int rychlay = 200; //nastavuji zakladni pozici strel
	int basex = 100; //nastavuji zakladni pozici strel
	int basey = 200; //nastavuji zakladni pozici strel
	Krajina k;
	Pozadi pozadi;
	Obrazek zbranraketa;
	Obrazek zbrankanon;
	Obrazek momentalnizbran;
	Menu hlavni;
	int naboj=1;
	int pocethracu=5;
	for (int vytvarim = 0; vytvarim < pocethracu; vytvarim++)
	{
		tanky.push_back(new Tank);
	}


	enum Stav {
		nic,
		//	zamerovani,
		//	strelba,
		//	vybuchovani,
		padani,
	};
	enum Stav stav = nic;
	auto a = tanky.begin();
	while(1)
	{
		//t1 = SDL_GetTicks();
		obrazovka->smaz();

		/* zacatek kresleni */
		//srand(1258);


		//rozmistuje tanky(snad pro libovolny pocet hracu)
		int i = 0;
		for (auto jakyumistuju: tanky)
		{
			int mezeraodokraje = 800 / pocethracu / 2;
			jakyumistuju->umisti(mezeraodokraje + i * 2 * mezeraodokraje, k.kdeJeHlina(mezeraodokraje + i * 2 * mezeraodokraje));
			i++;
		}


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



		for(v = vybuchy.begin(); v != vybuchy.end(); v++)
		{

			if((*v)->kresli(k) == true)
				stav = padani;
		}
		if(stav == padani)
			if(k.padej())
				stav = nic;


		for(s = strely.begin(); s != strely.end(); s++)
		{
			(*s)->kresli();
			if((*s)->pohni(&k))
			{
				vybuchy.push_back(new vybuch((*s)->x,(*s)->y,(*s)->vel));
				delete (*s);
				s = strely.erase(s);
				s--;
			}
		}

		//kresli tanky (snad pro libovolny pocet hracu)
		for (auto jakykreslim = tanky.begin(); jakykreslim != tanky.end(); jakykreslim++)
		{
			if((*jakykreslim)->zivoty <= 0)
			{
				vybuchy.push_back(new vybuch((*jakykreslim)->x, (*jakykreslim)->y, 40));
				if((*jakykreslim) == *a)
				{
					a++;
					if (a == tanky.end()) a = tanky.begin();
				}
				delete *jakykreslim;
				jakykreslim = tanky.erase(jakykreslim);
				jakykreslim--;


			}
		}
		for (auto jakykreslim: tanky)
		{
			jakykreslim->kresli(jakykreslim == *a);
		}



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
		if (naboj==3)
		{
			barva(100,255,118);
			for(int t=0; t<14; t++)
			{
				bod(750+t,30-sqrt(14*14-t*t));
			}
			for(int t=0; t<14; t++)
			{
				bod(750-t,30-sqrt(14*14-t*t));
			}
			for(int t=0; t<14; t++)
			{
				bod(726+t,30-sqrt(14*14-t*t));
			}
			kruh(721,15,5);
		}
		if (naboj==4)
		{
			barva(100,255,118);

		}

		/* konec kresleni */
		obrazovka->aktualizuj();

		Uint8* key = SDL_GetKeyState(NULL);
		if(key[SDLK_LEFT])
		{
			(*a)->vlevo();
		}
		if(key[SDLK_RIGHT])
		{
			(*a)->vpravo();
		}
		if(key[SDLK_DOWN])
		{
			if((*a)->prach<0.1)
			{
				(*a)->prach=0;
			}
			else
				(*a)->prach-=0.3;
		}
		if(key[SDLK_UP])
		{
			if((*a)->prach>40)
			{
				(*a)->prach=40;
			}
			else
				(*a)->prach+=0.3;
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
				case SDLK_4:
				case SDLK_KP4:
					naboj=4;
					break;
				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
				case SDLK_SPACE:
					{
						if (naboj==1)
						{
							strely.push_back(new strela((*a)->uhel, (*a)->prach/3, (*a)->x+20*2* cos((*a)->uhel), (*a)->y+20*2*sin((*a)->uhel)));
						}
						if (naboj==2)
						{
							strely.push_back(new raketa((*a)->uhel, (*a)->prach/3, (*a)->x+20*2* cos((*a)->uhel), (*a)->y+20*2*sin((*a)->uhel)));
						}
						if(naboj==3)
						{
							strely.push_back(new hopik((*a)->uhel, (*a)->prach/3, (*a)->x+20*2* cos((*a)->uhel), (*a)->y+20*2*sin((*a)->uhel)));
						}
						if(naboj==4)
						{
							strely.push_back(new bomba((*a)->uhel, (*a)->prach/3, (*a)->x+20*2* cos((*a)->uhel), (*a)->y+20*2*sin((*a)->uhel)));
						}

						a++;
						if (a == tanky.end()) a = tanky.begin();

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
