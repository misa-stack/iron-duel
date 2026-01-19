#ifndef MENU_H
#define MENU_H

#include <list>
#include <functional>
#include "grafika.h"

class Tlacitko
{
public:
	Tlacitko(const char* nazev, std::function<void()> funkce);
	void kresli(int x1, int y1, int x2, int y2);
	std::function<void()> funkce;
	const char* nazev;
	int klikx;
	int kliky;
	Pismo cislapismenamalaivelka;
	void klik();


};

class Menu
{
public:
	void klik(int kx, int ky);
	Menu();
	bool prvnitlacitko;
	int vzdalenostoddelenitlacitek;
	int pocettlacitek;
	int velikosttlaciteksirka;
	int velikosttlacitekvyska;
	int pocetrad;
	void pridej(Tlacitko *t);
	void kresli();

	std::list<Tlacitko*> tlacitka;

};

#endif // MENU_H
