#include "menu.h"
#include "strela.h"
#include <math.h>
#include <SDL/SDL.h>
#include "pozadi.h"
#include "krajina.h"
#include "tank.h"
#include "vybuch.h"
#include <list>
#include <functional>
;


Tlacitko::Tlacitko(const char* nazev, std::function<void()> funkce)
{

	this ->funkce = funkce;
	this ->nazev = nazev;
    cislapismenamalaivelka.nacti("cislapismenamalaivelka.png", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
}
void Tlacitko::kresli(int x1, int y1, int x2, int y2)
{

	barva(BILA);
	obdelnik(x1,y1,x2,y2);

	cislapismenamalaivelka.umisti(x1+5,y1);
	cislapismenamalaivelka.kresli(nazev);
}

Menu::Menu()
{
	prvnitlacitko=true;
	pocettlacitek=0;
	vzdalenostoddelenitlacitek =0;
    velikosttlaciteksirka = 150;
	velikosttlacitekvyska = 50;

}

void Menu::pridej(Tlacitko* t){
	tlacitka.push_back(t);
}
void Tlacitko::klik(){
	funkce();
}
void Menu::klik(int kx, int ky){
	int x = 100;
	int y = 0;
	int i = 0;



	pocettlacitek = tlacitka.size();
	for (auto t = tlacitka.begin(); t != tlacitka.end(); ++t)
	{
		if(pocettlacitek > 8)
			pocettlacitek=8;
		vzdalenostoddelenitlacitek =(600 - (velikosttlacitekvyska * pocettlacitek))/ (pocettlacitek+1);
		if(i>0)
			y += vzdalenostoddelenitlacitek  + velikosttlacitekvyska;
		else
			y += vzdalenostoddelenitlacitek;
		if ( i == 8){
			y = vzdalenostoddelenitlacitek ;
			x = 300;
		}
		if(kx >= x && kx <= x + velikosttlaciteksirka && ky >= y && ky <= y + velikosttlacitekvyska)
			(*t)->klik();
		i++;
	}

}
void Menu::kresli(){
    int x = 100;
	int y = 0;
	int i = 0;



	pocettlacitek = tlacitka.size();
	for (auto t = tlacitka.begin(); t != tlacitka.end(); ++t)
	{
		if(pocettlacitek > 8)
			pocettlacitek=8;
		vzdalenostoddelenitlacitek =(600 - (velikosttlacitekvyska * pocettlacitek))/ (pocettlacitek+1);
		if(i>0)
			y += vzdalenostoddelenitlacitek  + velikosttlacitekvyska;
		else
			y += vzdalenostoddelenitlacitek;
		if ( i == 8){
			y = vzdalenostoddelenitlacitek ;
            x = 300;
		}
		(*t)->kresli(x, y, x + velikosttlaciteksirka, y + velikosttlacitekvyska);
		i++;
	}






	/*for(t = tlacitka.begin(); t < tlacitka.size(); t++)
	{
		vzdalenostoddelenitlacitek  = (800 - pocettlacitek * velikosttlacitekvyska)/(pocettlacitek);
		if (prvnitlacitko == true){
			int y = y + vzdalenostoddelenitlacitek;
			prvnitlacitko = false;

		}
		if (prvnitlacitko == false){
			int y = y +vzdalenostoddelenitlacitek +velikosttlacitekvyska;
		}

		(*t)->kresli(x, y,x + velikosttlaciteksirka, y+velikosttlacitekvyska);
	}

		vzdalenostoddelenitlacitek = (800-50*pocettlacitek/2)/pocettlacitek;
	for(int i = 0; i < pocettlacitek; i++){
		barva(0,255,100);

		if (i == 0)
			prvnitlacitko=true;
		else
			prvnitlacitko=false;

			if(prvnitlacitko == true){
				obdelnik(50,i+20,50+velikosttlaciteksirka,i+velikosttlacitekvyska+20);

			}

		if(prvnitlacitko==false){
			obdelnik(50,i*vzdalenostoddelenitlacitek+20,50+velikosttlaciteksirka,i*vzdalenostoddelenitlacitek+20+velikosttlacitekvyska);
		}
	}*/
}

