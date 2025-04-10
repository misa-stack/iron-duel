#include "tank.h"
#include "grafika.h"
#include <math.h>

Tank::Tank()
{
	x=400;
	y=300;
	uhel=5;
	rk=20;
	o=0.01;
	prach=0;
	r=nahoda(255);
	g=nahoda(255);
	b=nahoda(255);

}

void Tank::kresli()
{
	barva(r, g, b);
	for(int t=0; t<rk; t++)
	{
		bod(x+t,y);
		cara(x+t,y-sqrt(rk*rk-t*t));
	}
	for(int t=0; t<rk; t++)
	{
		bod(x-t,y);
		cara(x-t,y-sqrt(rk*rk-t*t));
	}
	obdelnik(x-rk, y, x+rk, y+rk/2 );
	cara(x, y, x+rk*2* cos(uhel), y+rk*2*sin(uhel));

}

void Tank::naloz()
{
	barva(BILA);
	for (int a=0; a<5; a++)
	{
		cara(x-20, y-45-a, x-20+prach, y-45-a);
	}

}


void Tank::vlevo()
{
	if(uhel<3.2)
	{
		uhel==3.2;
	}
	else
		uhel-=o;
}

void Tank::vpravo()
{
	if(uhel>6.2)
	{
		uhel==6.2;
	}
	else
		uhel+=o;
}

void Tank::umisti(int xu, int yu)
{
	x=xu;
	y=yu;
}

