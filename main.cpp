#include "menu.h"
#include "strela.h"
#include <math.h>
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "tank.h"
#include "vybuch.h"
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
	vybuch v(100,200);
	Krajina k;
	Pozadi pozadi;
    Obrazek zbranraketa;
    Obrazek zbrankanon;
    Obrazek momentalnizbran;
Menu hlavni;
    int tah=0;

    Tank tank[2];
    tank[1].umisti(130, k.kdeJeHlina(130));
    tank[0].umisti(670, k.kdeJeHlina(670));

    strela*s=NULL;

    while(1)
    {
        //t1 = SDL_GetTicks();
        obrazovka->smaz();

        /* zacatek kresleni */
        //srand(1258);

        int a=tah;

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

	v.kresli();

      hlavni.kresli();
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
		/* konec kresleni */
		obrazovka->aktualizuj();

        }

	}

}
