#include "hra.h"
#include "menu.h"
#include "main.h"
#include "raketa.h"
#include "hopik.h"
#include "bomba.h"
#include "ultimatnizbran.h"


Hra::Hra()
{
	Koneckola = new Ukazatel("Konec kola", kolo);
	ukazatelHracu = new Ukazatel("Pocet hracu", pocethracu);
}

void Hra::kresli()
{
	// pohyb a vykreslení pozadí + krajiny
        pozadi.pohni();
        pozadi.aktualizuj();
        pozadi.kresli();
        k.kresli();

	// hra
        if (aktivni == NULL) {
            // korekce pozic tanků na terén
            // (pro případ, že terén padá nebo se změnil)
            for (auto t : tanky) {
                t->umisti(t->x, k.kdeJeHlina(t->x));
            }

            vykresliIkonuNaboje(naboj);

            // výbuchy a padání terénu
            // střely a jejich kolize
            aktualizujStrelyAKolize(k, stav);
            aktualizujVybuchyAKrajinu(k, stav);

            // vyřaď mrtvé tanky (vytvoř výbuch, přepni hráče korektně)
            for (auto it = tanky.begin(); it != tanky.end();) {
                Tank* t = *it;
                if (t->zivoty <= 0) {
                    vybuchy.push_back(new vybuch(t->x, t->y, 40));
                    bool mazanyJeAktivni = (t == *a);
		    t->penize += 100;
		    //delete t;
		    //1000-pocethracu*100+klk_hracu_uz_umrelo*100
		     it = tanky.erase(it);
                    if (tanky.empty()) {
                        // všichni mrtví – konec hry
                        aktivni = &ekonomicke;
                        vybuchy.clear();
                        strely.clear();
                        Koneckola->kresli(450,250,550,300);

                        k.zmena();
                        kolo++;
                        if (kolo == 3){
                            kolo = 0;
                            aktivni = &hlavni;
                        }
                        break;
                    }

                        if (mazanyJeAktivni) {
                            a = tanky.begin(); // posuň aktivního na validní
                    }
                } else {
                    ++it;
                }
            }

            // vykresli tanky (aktivní = zvýrazněný)
            for (auto it = tanky.begin(); it != tanky.end(); ++it) {
                bool jeAktivni = ((*it) == (*a));
                (*it)->kresli(jeAktivni);
            }
        }
        else{
        aktivni->kresli();
        if (aktivni == &vyberove_na_hru) {
            vyberove_na_hru.kresli();
            pocethracu--;
            ukazatelHracu->kresli(900,20,1050,70);
            pocethracu++;}
        }	
}

void Hra::nova()
{
	// založení tanků
	if (kolo == 0){
		for (int vytvarim = 0; vytvarim < pocethracu; vytvarim++) {
			Tank* nt = new Tank;
			tanky.push_back(nt);
			tanky_penize.push_back(nt);
		}
	}
	else{
		for (auto it = tanky_penize.begin(); it != tanky_penize.end(); ++it) {
			(*it)->zivoty = 100;
			tanky.push_back(*it);
		}
	}


	a = tanky.begin();
	rozmistitTanky(k, pocethracu);
}

void Hra::klavesa(unsigned int sym)
{
	switch(sym)
	{
	case SDLK_1:
	case SDLK_KP1:
		naboj = 1; break;
	case SDLK_2:
	case SDLK_KP2:
		naboj = 2; break;
	case SDLK_3:
	case SDLK_KP3:
		naboj = 3; break;
	case SDLK_4:
	case SDLK_KP4:
		naboj = 4; break;
	case SDLK_KP5:
	case SDLK_5:
		naboj = 5; break;
	case SDLK_SPACE:
		if (aktivni == NULL && !tanky.empty()) {
			vystrelNaboj(naboj, *a);
			++a;
			if (a == tanky.end()) a = tanky.begin();
		}
		break;
		
	}
}
void Hra::zpracujKlavesyTanku(const Uint8* key, Tank* t)
{
    if (key[SDLK_LEFT])  t->vlevo();
    if (key[SDLK_RIGHT]) t->vpravo();

    if (key[SDLK_DOWN]) {
        t->prach = (t->prach < 0.1) ? 0.0 : (t->prach - 0.3);
    }
    if (key[SDLK_UP]) {
        t->prach = (t->prach > 40.0) ? 40.0 : (t->prach + 0.3);
    }
}

void Hra::vystrelNaboj(int naboj, Tank* t)
{
    double ux = t->x + 40.0 * cos(t->uhel);
    double uy = t->y + 40.0 * sin(t->uhel);
    double sila = t->prach / 3.0;

    switch (naboj) {
    case 1:
        strely.push_back(new strela(t->uhel, sila, ux, uy));
        break;
    case 2:
        strely.push_back(new raketa(t->uhel, sila, ux, uy));
        break;
    case 3:
        strely.push_back(new hopik(t->uhel, sila, ux, uy));
        break;
    case 4:
        strely.push_back(new bomba(t->uhel, sila, ux, uy));
        break;
    case 5:
        strely.push_back(new ultimatnizbran(t->uhel, sila, ux, uy));


    }
}

bool Hra::aktualizujStrelyAKolize(Krajina& k, enum Stav& stav)
{
    bool necoVybusne = false;

    for (s = strely.begin(); s != strely.end();) {
        strela* pr = *s;
        pr->kresli();

        if (pr->pohni(&k)) {
            vybuchy.push_back(new vybuch(pr->x, pr->y, pr->vel));
            delete pr;
            s = strely.erase(s);
            necoVybusne = true;
        }
        else {
            ++s;
        }
    }

    if (necoVybusne) stav = padani;
    return necoVybusne;
}

void Hra::aktualizujVybuchyAKrajinu(Krajina& k, enum Stav& stav)
{
    for (v = vybuchy.begin(); v != vybuchy.end(); ++v) {
        if ((*v)->kresli(k)) {
            stav = padani;
        }
    }
    if (stav == padani) {
        if (k.padej()) {
            stav = nic;
        }
    }
}

void Hra::rozmistitTanky(Krajina& k, int pocethracu)
{
    if (tanky.empty()) return;

    int mezeraodokraje = 1067 / pocethracu / 2;
    int i = 0;
    for (auto t : tanky) {
        int x = mezeraodokraje + i * 2 * mezeraodokraje;
        t->umisti(x, k.kdeJeHlina(x));
        i++;
    }
    // korekce výšky po případném padání terénu
    for (auto t : tanky) {
        t->umisti(t->x, k.kdeJeHlina(t->x));
    }
}

void Hra::vykresliIkonuNaboje(int naboj)
{
    barva(100, 255, 118);
    switch (naboj) {
    case 1:
        kruh(750, 20, 5);
        break;
    case 2:
        trojuhelnik(740, 15, 745, 30, 735, 30);
        break;
    case 3: {
        for (int t = 0; t < 14; t++) {
            bod(750 + t, 30 - sqrt(14 * 14 - t * t));
            bod(750 - t, 30 - sqrt(14 * 14 - t * t));
            bod(726 + t, 30 - sqrt(14 * 14 - t * t));
        }
        kruh(721, 15, 5);
        break;
    }
    case 4:
        kruh(750, 15, 5);
        kruh(730, 15, 5);
        kruh(770, 15, 5);
        break;
    case 5:
        break;

    }
}
