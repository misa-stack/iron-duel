#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "raketa.h"



void kometa(int x, int y,int r){
	kruh(x,y,r);

}
int main(int argc, char** argv)
{
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);
	strela base(100,200);
	Krajina k;
	Pozadi pozadi;
	raketa rychla(100,200);
	//int t2;
	//int t1;
	bool bas = false;
	bool rychl = false;
	int s =1;
	Obrazek zbranraketa;
	Obrazek zbrankanon;
	Obrazek momentalnizbran;


	while(1)
	{
		//t1 = SDL_GetTicks();
		obrazovka->smaz();

		/* zacatek kresleni */
		//srand(1258);




		pozadi.pohni();
		pozadi.kresli();
		k.kresli();
		if(bas ==true){
			base.kresli();
			base.pohni();
		}
		if(rychl == true){
			rychla.kresli();
			rychla.pohni();
		}
		momentalnizbran.nacti("momentalnizbran.png");
		momentalnizbran.umisti(600,0);
		momentalnizbran.kresli();
		if(s == 1)
		{
			zbrankanon.nacti("kanon.png");
			zbrankanon.umisti(700,10);
			zbrankanon.kresli();
		}
		if(s == 2)
		{
			zbranraketa.nacti("raketa.png");
			zbranraketa.umisti(700,10);
			zbranraketa.kresli();
		}

		/* konec kresleni */
		obrazovka->aktualizuj();
		//SDL_Delay(500);

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
					s++;
					if(s == 3){
						s= 1;
					}
					break;
				case SDLK_f:
					if(s==1){

						bas = true;
						break;
					}
					if(s==2){
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

