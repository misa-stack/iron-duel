#include "menu.h"
#include "strela.h"
#include <math.h>
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "tank.h"
#include "vybuch.h"

Menu::Menu()
{
	prvnitlacitko=true;
	pocettlacitek=10;
	vzdalenostoddelenitlacitek =0;
}

void Menu::pridej(){
	pocettlacitek++;

}
void Menu::kresli(){
	vzdalenostoddelenitlacitek = (800-50*pocettlacitek/2)/pocettlacitek;
	for(int i = 0; i < pocettlacitek; i++){







		barva(0,255,100);
if (i == 0)
	prvnitlacitko=true;
else
	prvnitlacitko=false;
		if(prvnitlacitko == true){
			obdelnik(400,i+20,450,i+45);

		}
		if(prvnitlacitko==false){
			obdelnik(400,i*vzdalenostoddelenitlacitek+20,450,i*vzdalenostoddelenitlacitek+45);
		}
	}
}
