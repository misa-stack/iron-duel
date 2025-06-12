#include "hopik.h"
#include "math.h"

hopik::hopik(float uhel, float prach, float x,float y): strela(uhel, prach, x, y)
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

	return false;
}
