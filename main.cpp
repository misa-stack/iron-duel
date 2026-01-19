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

void kometa(int x, int y, int r)
{
    kruh(x, y, r);
}

std::list<Tank*> tanky;
std::list<Tank*>::iterator a;
std::list<strela*> strely;
std::list<strela*>::iterator s;
std::list<vybuch*> vybuchy;
std::list<vybuch*>::iterator v;

enum Stav {
    nic,
    padani,
};

static inline void rozmistitTanky(Krajina& k, int pocethracu)
{
    if (tanky.empty()) return;

    int mezeraodokraje = 800 / pocethracu / 2;
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

static inline void vykresliIkonuNaboje(int naboj)
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
    }
}

static inline void zpracujKlavesyTanku(const Uint8* key, Tank* t)
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

static inline void vystrelNaboj(int naboj, Tank* t)
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
    }
}

static inline bool aktualizujStrelyAKolize(Krajina& k, enum Stav& stav)
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
        } else {
            ++s;
        }
    }

    if (necoVybusne) stav = padani;
    return necoVybusne;
}

static inline void aktualizujVybuchyAKrajinu(Krajina& k, enum Stav& stav)
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

int main(int argc, char** argv)
{
    Obrazovka* obrazovka = Obrazovka::instance();
    obrazovka->inicializuj(800, 600, 0, 0);

    Krajina k;
    Pozadi pozadi;

    Obrazek momentalnizbran;
    bool momentalnizbran_nacten = false;

    bool zakladni_menu = true;
    bool vyberove_menu = false;
    bool hra = false;
    int naboj = 1;
    int pocethracu = 5;

    Menu hlavni;
    hlavni.pridej(new Tlacitko("nova hra", [&]() {
        zakladni_menu = false;
        vyberove_menu = true;
    }));

    hlavni.pridej(new Tlacitko("konec hry", []() {
        SDL_Quit();
        // žádné return zde – nechť ukončí main
    }));

    hlavni.pridej(new Tlacitko("zmena pozadi", [&]() {
        pozadi.aktualizuj();
    }));

    hlavni.pridej(new Tlacitko("zmena terenu", [&]() {
        k.zmena();
    }));

    Menu vyberove_na_hru;
    vyberove_na_hru.pridej(new Tlacitko("pocet hracu 1", [&]() {
        pocethracu = 2;
    }));
    vyberove_na_hru.pridej(new Tlacitko("pocet hracu 2", [&]() {
        pocethracu = 3;
    }));
    vyberove_na_hru.pridej(new Tlacitko("pocet hracu 3", [&]() {
        pocethracu = 4;
    }));
    vyberove_na_hru.pridej(new Tlacitko("pocet hracu 4", [&]() {
        pocethracu = 5;
    }));

    vyberove_na_hru.pridej(new Tlacitko("zacit hru", [&]() {
        hra = true;
        vyberove_menu = false;

        // založení tanků
        for (int vytvarim = 0; vytvarim < pocethracu; vytvarim++) {
            tanky.push_back(new Tank);
        }
        a = tanky.begin();
        rozmistitTanky(k, pocethracu);

        // načti jednou – ne při každém snímku
        if (!momentalnizbran_nacten) {
            momentalnizbran.nacti("momentalnizbran.png");
            momentalnizbran.umisti(600, 0);
            momentalnizbran_nacten = true;
        }
    }));

    enum Stav stav = nic;

    // hlavní smyčka
    bool bezi = true;
    while (bezi) {
        obrazovka->smaz();
        int cas1 = SDL_GetTicks();

        // pohyb a vykreslení pozadí + krajiny
        pozadi.pohni();
        pozadi.aktualizuj();
        pozadi.kresli();
        k.kresli();

        // menu
        if (zakladni_menu) {
            hlavni.kresli();
        } else if (vyberove_menu) {
            vyberove_na_hru.kresli();
        }

        // hra
        if (hra) {
            // korekce pozic tanků na terén
            // (pro případ, že terén padá nebo se změnil)
            for (auto t : tanky) {
                t->umisti(t->x, k.kdeJeHlina(t->x));
            }

            // vykresli panel aktuální zbraně (jednou načtený obrázek)
            if (momentalnizbran_nacten) {
                momentalnizbran.kresli();
            }
            vykresliIkonuNaboje(naboj);

            // výbuchy a padání terénu
            aktualizujVybuchyAKrajinu(k, stav);

            // střely a jejich kolize
            aktualizujStrelyAKolize(k, stav);

            // vyřaď mrtvé tanky (vytvoř výbuch, přepni hráče korektně)
            for (auto it = tanky.begin(); it != tanky.end();) {
                Tank* t = *it;
                if (t->zivoty <= 0) {
                    vybuchy.push_back(new vybuch(t->x, t->y, 40));
                    bool mazanyJeAktivni = (t == *a);
                    delete t;
                    it = tanky.erase(it);

                    if (tanky.empty()) {
                        // všichni mrtví – konec hry
                        hra = false;
                        vyberove_menu = false;
                        zakladni_menu = true;
                        vybuchy.clear();
                        strely.clear();
                        k.zmena();
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
        int cas2 = SDL_GetTicks();
        SDL_Delay(40-(cas2-cas1));
        obrazovka->aktualizuj();

        // vstupy z kláves – plynulé (držení)
        Uint8* key = SDL_GetKeyState(NULL);
        if (hra && !tanky.empty()) {
            zpracujKlavesyTanku(key, *a);
        }

        // události
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (zakladni_menu) {
                    hlavni.klik(event.button.x, event.button.y);
                } else if (vyberove_menu) {
                    vyberove_na_hru.klik(event.button.x, event.button.y);
                }
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
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

                case SDLK_ESCAPE:
                    bezi = false;
                    break;

                case SDLK_SPACE:
                    if (hra && !tanky.empty()) {
                        vystrelNaboj(naboj, *a);
                        ++a;
                        if (a == tanky.end()) a = tanky.begin();
                    }
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
    }

    SDL_Quit();
    return 0;
}
