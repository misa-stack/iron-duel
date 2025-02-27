#include "vybuch.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

vybuch::vybuch(int x, int y): xk(x), yk(y)
{
<<<<<<< HEAD
rk = 1;
=======
	rk = 1;
>>>>>>> d3d26643b0205b04442dfc0af7cea6e2cc13b8f5
}

void vybuch::kresli()
{
	barva(237,98,64);
	kruh(xk,yk,rk);
    barva(237,98,64);
    kruh(xk,yk,rk);
<<<<<<< HEAD
    if (rk < 100 & rk>0)
    {
        rk ++;
    }
    else
    {
        rk = 0;
    }
=======
    rk++;
	if (rk < 30 & rk > 0) {
		rk++;
	}
	else{

		rk = 0;
	}
>>>>>>> d3d26643b0205b04442dfc0af7cea6e2cc13b8f5
}

