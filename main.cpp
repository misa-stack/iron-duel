#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
void kometa(int x, int y,int r){
	kruh(x,y,r);

}
int main(int argc, char** argv)
{
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);

	Krajina k;
	Pozadi pozadi;
	while(1)
	{
		obrazovka->smaz();
		
		/* zacatek kresleni */
		//srand(1258);
		pozadi.pohni();
		pozadi.kresli();
		k.kresli();

		/* konec kresleni */
		obrazovka->aktualizuj();
		SDL_Delay(500);
		
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_u:
					pozadi.zmenu();
					break;
				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
				}
				break;
			}
		}
	}
}
