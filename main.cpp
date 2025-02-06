#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "vybuch.h"
void kometa(int x, int y,int r){
	kruh(x,y,r);

}
int main(int argc, char** argv)
{
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);
	strela base(100,200);
    vybuch v(100,200);
	Krajina k;
	Pozadi pozadi;
	//int t2;
	//int t1;
	while(1)
	{
		//t1 = SDL_GetTicks();
		obrazovka->smaz();
		
		/* zacatek kresleni */
		//srand(1258);
		pozadi.pohni();
		pozadi.kresli();
		base.kresli();
		base.pohni();
		k.kresli();
        v.kresli();
        v.rk++;

		/* konec kresleni */
		obrazovka->aktualizuj();
    //	SDL_Delay(500);
		
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
				case SDLK_s:
					base.zmena();
					break;
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

