#include "vybuch.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

vybuch::vybuch(int x, int y): xk(x), yk(y)
{
	rk = 1;
}

void vybuch::kresli()
{
	barva(237,98,64);
	kruh(xk,yk,rk);

	if (rk < 30 & rk > 0) {
		rk++;
	}
	else{

		rk = 0;
	}
}

