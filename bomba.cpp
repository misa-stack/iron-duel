#include "ultimatnizbran.h"
#include "math.h"
#include "bomba.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "strela.h"
#include "raketa.h"
#include "main.h"


bomba::bomba(float uhel, float prach, float x,float y): strela(uhel, prach, x, y)
{
}

void bomba::kresli()
{
    barva(100,255,118);
    kruh(x,y,7);
}

bool bomba::pohni(Krajina *k)
{
    x= x + vx;
    y= y + vy;
    vy= vy+ ay;
    if(k->jeHlina(x, y))
    {
        for (int c=0; c<20; c++)
        {
            hra->strely.push_back(new strela(-nahoda(3.14), 5, x-vx, y-vy));
        }
        return true;
    }
    else{
        return false;}
}

