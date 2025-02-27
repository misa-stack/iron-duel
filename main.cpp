#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "raketa.h"
#include "vybuch.h"

int main(int argc, char** argv)
{
    int rychlax = 100;
    int rychlay = 200;
    int basex = 100;
    int basey = 200;
    Obrazovka* obrazovka = Obrazovka::instance();
    obrazovka->inicializuj(800, 600, 0, 0);
    strela base(basex,basey);
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
        if (v.rk > 100)
        {
            v.rk = 0;
        }
    Krajina k;
    Pozadi pozadi;
    raketa rychla(rychlax,rychlay);
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

	//v.kresli();
	//v.rk++;
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
}

