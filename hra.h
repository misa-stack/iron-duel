#ifndef HRA_H
#define HRA_H

#include <list>
#include "tank.h"
#include "vybuch.h"
#include "strela.h"
#include "pozadi.h"
#include "menu.h"

enum Stav {
    nic,
    padani,
};

class Hra
{
public:
	Krajina k;
	Pozadi pozadi;

	Hra();
	
	void kresli();
	void nova();
	void klavesa(unsigned int sym);
	
	enum Stav stav = nic;

	static const int max_pocet_hracu = 9;
	int naboj = 1;
	int kolo = 0;
	int pocethracu = 5;

	Ukazatel* Koneckola;
	Ukazatel* ukazatelHracu;
	
	std::list<Tank*> tanky;
	std::list<Tank*>::iterator a;
	Tank* tanky_penize[max_pocet_hracu];
	std::list<strela*> strely;
	std::list<strela*>::iterator s;
	std::list<vybuch*> vybuchy;
	std::list<vybuch*>::iterator v;
	
	void zpracujKlavesyTanku(const Uint8* key, Tank* t);
	void vystrelNaboj(int naboj, Tank* t);
	bool aktualizujStrelyAKolize(Krajina& k, enum Stav& stav);
	void aktualizujVybuchyAKrajinu(Krajina& k, enum Stav& stav);
	void rozmistitTanky(Krajina& k, int pocethracu);
	void vykresliIkonuNaboje(int naboj);
	
	
};

#endif // HRA_H
