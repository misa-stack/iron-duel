#include "vybuch.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

vybuch::vybuch(int x, int y): xk(x), yk(y)
{
rk = 1;
	rk = 1;

	rk = 29;
}

void vybuch::kresli()
{
	barva(237,98,64);
	kruh(xk,yk,rk);
    barva(237,98,64);
    kruh(xk,yk,rk);


	if (rk < 30 & rk > 0) {
		rk = rk -1 ;
	}
	else{

		rk = 0;
	}
}

