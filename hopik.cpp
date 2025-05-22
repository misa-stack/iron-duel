#include "hopik.h"

hopik::hopik(float x, float y):strela(x, y)
{
}

bool hopik::pohni(Krajina *k)
{

	//hejbu se strelou
	x= x + vx;
	y= y + vy;
	vy= vy+ ay;
	if(k->jeHlina(x, y))
	{



		vy=-vy;
		x= x + vx;
		y= y + vy;

		vy*=0.5;
		pocet++;
		if(pocet>3)
		{
		return true;
		}
	}

	return false;
}
