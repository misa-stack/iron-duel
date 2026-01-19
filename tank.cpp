#include "tank.h"
#include "grafika.h"
#include <math.h>
#include "strela.h"
#include <SDL/SDL.h>
#include <list>

extern std::list<Tank*> tanky;
extern std::list<strela*> strely;

Tank::Tank()
{
	x=0;
	y=0;
	uhel=5;
	rk=20;
	o=0.01;
	prach=0;
	r=nahoda(255);
	g=nahoda(255);
	b=nahoda(255);

}

void Tank::kresli(bool aktivni)
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

	if(aktivni) //kresli caru sily vystreli
	{
		barva(BILA);
		for (int a=0; a<5; a++)
		{
			cara(x-20, y-45-a, x-20+prach, y-45-a);
		}
	}

	else //kresli hp bar
	{
		barva(CERVENA);
		for (int a=0; a<5; a++)
		{
			cara(x-20, y-45-a, x-20+zivoty/2.5, y-45-a);
		}
	}
}

void Tank::naloz()
{


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
	if (yu == y+1)
	{
		zivoty = zivoty -1;
	}
	x=xu;
	y=yu;

}


