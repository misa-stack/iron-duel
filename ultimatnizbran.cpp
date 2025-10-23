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

ultimatnizbran::ultimatnizbran(float uhel, float prach, float x, float y): strela(uhel, prach, x, y)
{

}

bool ultimatnizbran::pohni(Krajina *k)  {

    //hejbu se strelou
    x= x + vx;
    y= y + vy;
    vy= vy+ ay * 0,4;
    if(k->jeHlina(x, y))
    {
        for (int c=0; c<20; c++)
        {
            hra->strely.push_back(new raketa(-nahoda(3.14), 5, x-vx, y-vy));
        }
        float x1 = x-10;
        float y1 = k->kdeJeHlina(x1);
        float x2 = x+10;
        float y2 = k->kdeJeHlina(x2);

        float sx = (x2 - x1);
        float sy = (y2 - y1);
        float nx = sy;
        float ny = -sx;
        float nSize = sqrt((nx*nx) + (ny*ny));
        float n1x = nx/nSize;
        float n1y = ny/nSize;
        float vDotN1 = vx*n1x + vy*n1y;
        float v1x = vx - 2 * vDotN1 * n1x;
        float v1y = vy - 2 * vDotN1 * n1y;



        vx = v1x;
        vy = v1y;
        x = x + vx;
        y = y + vy;

        vy*=0.5;
        pocet++;
        if(pocet>3)
        {
            return true;
        }
    }

    x = x + vx;
    y = y + vy;
    vy = vy + ay;

    if (x < 0)
    {
        x = 0;
        vx = -vx  ;
        for (int c=0; c<40; c++)
        {
            hra->strely.push_back(new strela(-nahoda(3.14), 5, x-vx, y-vy));
        }
        return true;
    }

    if (x > 1067)
    {
        x = 1067;
        vx = -vx ;
        for (int c=0; c<40; c++)
        {
            hra->strely.push_back(new strela(-nahoda(3.14), 5, x-vx, y-vy));
        }
        return true;
    }

    if (y < 0)
    {
        y = 0;
        vy = -vy ;
        for (int c=0; c<40; c++)
        {
            hra->strely.push_back(new strela(-nahoda(3.14), 5, x-vx, y-vy));
        }
        return true;
    }

    if (y > 600)
    {
        y = 600;
        vy = -vy ;
        for (int c=0; c<40; c++)
        {
            hra->strely.push_back(new strela(-nahoda(3.14), 5, x-vx, y-vy));
        }
        return true;
    }

    return false;
}

