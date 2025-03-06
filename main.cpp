#include "strela.h"
#include <math.h>
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "tank.h"
void kometa(int x, int y,int r){
	kruh(x,y,r);

}
int main(int argc, char** argv)
{
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(800, 600, 0, 0);

    int tah=0;

    Tank tank[2];
    tank[1].umisti(130, 300);
    tank[0].umisti(670, 300);

    strela*s=NULL;
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
        int a=tah;
		pozadi.pohni();
		pozadi.kresli();
        k.kresli();

        if(s)
        {
            s->kresli();
            s->pohni();
        }

        tank[1].kresli();
        tank[0].kresli();
        tank[a].naloz();

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
				case SDLK_k:
					k.zmena();
					break;

				case SDLK_ESCAPE:
					SDL_Quit();
					return 0;
                case SDLK_SPACE:
                    s = new strela(tank[a].x+20*2*cos(tank[a].uhel), tank[a].y+20*2*sin(tank[a].uhel));
                    s->vx=tank[a].prach/21*cos(tank[a].uhel);
                    s->vy=tank[a].prach/21*sin(tank[a].uhel);

                    if (tah==1)
                    {
                        tah--;
                    }
                    else
                        tah++;
                    break;
				}
				break;
			}
		}
		//t2 = SDL_GetTicks();
		//int dt = t2 - t1;
		//if (5 - dt > 0)SDL_Delay(5 -dt);
	}
}

