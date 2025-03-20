#include "strela.h"
#include <math.h>
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "tank.h"
#include "raketa.h"

void kometa(int x, int y,int r)
{
    kruh(x,y,r);
}
int main(int argc, char** argv)
{
    Obrazovka* obrazovka = Obrazovka::instance();
    obrazovka->inicializuj(800, 600, 0, 0);

    int tah=0;
    int naboj=1;

    Tank tank[2];
    tank[1].umisti(130, 300);
    tank[0].umisti(670, 300);

    strela*s=NULL;
    Krajina k;
    Pozadi pozadi;

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
                case SDLK_k:
                    k.zmena();
                    break;
                case SDLK_1:
                case SDLK_KP1:
                    naboj=1;
                    break;
                case SDLK_2:
                case SDLK_KP2:
                    naboj=2;
                    break;
                case SDLK_ESCAPE:
                    SDL_Quit();
                    return 0;
                case SDLK_SPACE:
                    if (naboj==1)
                    {
                        s = new strela(tank[a].x+20*2*cos(tank[a].uhel), tank[a].y+20*2*sin(tank[a].uhel));

                    }
                    else
                        s = new raketa(tank[a].x+20*2*cos(tank[a].uhel), tank[a].y+20*2*sin(tank[a].uhel));

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
                break;

            }
        }
        //t2 = SDL_GetTicks();
        //int dt = t2 - t1;
        //if (5 - dt > 0)SDL_Delay(5 -dt);
    }
}
