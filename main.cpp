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
#include "ultimatnizbran.h"
#include "hra.h"


void kometa(int x, int y, int r)
{
	kruh(x, y, r);
}

int fpspocet = 30;

Menu* aktivni;
Menu hlavni;
Menu nastaveni;
Menu vyberove_na_hru;
Menu ekonomicke;
Hra *hra;

bool fullscreen = false;
int main(int argc, char** argv)
{
	
	Obrazovka* obrazovka = Obrazovka::instance();
	obrazovka->inicializuj(1067, 600, 0, fullscreen ? SDL_FULLSCREEN : 0);
	
	hra = new Hra();
	

	aktivni = &hlavni;
	
	ekonomicke.pridej(new Tlacitko("pokracovat ve hre",[&](){
        aktivni = NULL;
		
		hra->nova();
	}));
	
	hlavni.pridej(new Tlacitko("nova hra", [&]() {
		aktivni = &vyberove_na_hru;
	}));
	
	
	hlavni.pridej(new Tlacitko("nastaveni",[&](){
		aktivni = &nastaveni;
	}));
	
	
	
	hlavni.pridej(new Tlacitko("konec hry", []() {
		SDL_Quit();
		// žádné return zde – nechť ukončí main
	}));
	nastaveni.pridej(new Tlacitko("fullscreen",[&](){
		fullscreen = !fullscreen;
		obrazovka->inicializuj(1067, 600, 0, fullscreen ? SDL_FULLSCREEN : 0);
	}));
	nastaveni.pridej(new Tlacitko("zpet",[&](){
		aktivni = &hlavni;
	}));
	
	vyberove_na_hru.pridej(new Tlacitko("mene hracu", [&]() {
		if (hra->pocethracu > 2) hra->pocethracu--;
	}));
	
	vyberove_na_hru.pridej(new Tlacitko("vice hracu", [&]() {
		if (hra->pocethracu < hra->max_pocet_hracu) hra->pocethracu++;
	}));
	vyberove_na_hru.pridej(new Tlacitko("zmena terenu", [&]() {
		hra->k.zmena();
	}));
	
	vyberove_na_hru.pridej(new Tlacitko("zacit hru", [&]() {
		aktivni = NULL;
		
		hra->nova();
	}));
	vyberove_na_hru.pridej(new Tlacitko("zpet", [&]() {
		aktivni = &hlavni;
	}));
	
	
	// hlavní smyčka
	bool bezi = true;
	while (bezi) {
		obrazovka->smaz();
		int cas1 = SDL_GetTicks();
		
		hra->kresli();
		
		// menu
		
		
		
		obrazovka->aktualizuj();
		
		// vstupy z kláves – plynulé (držení)
		Uint8* key = SDL_GetKeyState(NULL);
		if (aktivni == NULL && !hra->tanky.empty()) {
			hra->zpracujKlavesyTanku(key, *hra->a);
		}
		
		// události
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				if(aktivni)
					aktivni->klik(event.button.x,event.button.y);
				break;
				
			case SDL_KEYDOWN:
				hra->klavesa(event.key.keysym.sym);
				switch (event.key.keysym.sym) {
					
					
				case SDLK_ESCAPE:
					bezi = false;
					break;
					
				
				default:
					break;
				}
				break;
				
			case SDL_QUIT:
				bezi = false;
				break;
				
			default:
				break;
			}
		}
		int cas2 = SDL_GetTicks();
		
		if(cas2-cas1<17)
			SDL_Delay(17-(cas2-cas1));
		
	}
	
	SDL_Quit();
	return 0;
}
