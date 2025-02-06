#include "krajina.h"
#include "grafika.h"

#include <time.h>

Krajina::Krajina()
{
	srand(time (NULL));
	typ = 0;

}

// void terencara(int x1, int y1, int x2, int y2)
// {
//     double rozptyl = (x2 - x1)/ 3;
//     int xs = (x1 + x2)/2;
//     int ys = (y1 + y2)/2 + nahoda(rozptyl) - rozptyl/2;

//     if(x2 - x1 < 10)
//     {

//         cara(x1,y1,xs,ys);
//         cara(xs,ys,x2,y2);
//     }
//     else
//     {
//         terencara(x1,y1,xs,ys);
//         terencara(xs,ys,x2,y2);
//     }
// }
void Krajina::mojecara(int x1, int y1, int x2, int y2)
{
	float y = y1;
	float k = float(y2-y1)/float(x2-x1);
	for(int x = x1; x < x2; x++)
	{
		bod(x,y);
		cara(x,599);
		y += k;
	}
}
void Krajina::kopec(int x1, int y1, int x2, int y2)
{
	int rozptyl = (x2 - x1)/ 3;
	int xs = (x1 + x2)/2;
	int ys = (y1 + y2)/2 + nahoda(rozptyl) - rozptyl/2;

	if(x2 - x1 < 10)
	{

		mojecara(x1,y1,xs,ys);
		mojecara(xs,ys,x2,y2);
	}
	else
	{
		kopec(x1,y1,xs,ys);
		kopec(xs,ys,x2,y2);
	}
}
void Krajina::poust(int x1, int y1, int x2, int y2){
	{
		int rozptyl = (x2 - x1)/5;
		int xs = (x1 + x2)/2;
		int ys = (y1 + y2)/2+ nahoda(rozptyl) - rozptyl/2;


		if(x2 - x1 < 5)
		{

			mojecara(x1,y1,xs,ys);
			mojecara(xs,ys,x2,y2);
		}
		else
		{
			poust(x1,y1,xs,ys);
			poust(xs,ys,x2,y2);
		}
	}
}
void Krajina::hory(int x1, int y1, int x2, int y2)
{
	int rozptyl = (x2 - x1);
	int xs = (x1 + x2)/2;
	int ys = (y1 + y2)/2+ nahoda(rozptyl) - rozptyl/2;

	while(ys < 50 || ys > 600)
	{
		ys = (y1 + y2)/2+ nahoda(rozptyl) - rozptyl/2;
	}
	if(x2 - x1 < 50)
	{

		mojecara(x1,y1,xs,ys);
		mojecara(xs,ys,x2,y2);
	}
	else
	{
		hory(x1,y1,xs,ys);
		hory(xs,ys,x2,y2);
	}
}

void Krajina::kresli()
{
srand(13);
    if(typ == 0)
    {
        barva(65,152,10);
    kopec(0, nahoda(400) + 200,799, nahoda(400) + 200);
    }
    if (typ ==1)
    {
        barva(130,130,130);
        hory(0, nahoda(400) + 200,799, nahoda(400) + 200);
    }
    if (typ == 2){
barva(223,226,127);
poust(0,nahoda(400)+200,799,nahoda(400)+200);
    }

	srand(13);
	if(typ == 0)
	{
		barva(65,152,10);
		kopec(0, nahoda(400) + 200,799, nahoda(400) + 200);
	}
	if (typ ==1)
	{
		barva(130,130,130);
		hory(0, nahoda(400) + 200,799, nahoda(400) + 200);
	}
	if (typ == 2){
		barva(223,226,127);
		poust(0,nahoda(400)+200,799,nahoda(400)+200);
	}

}
void Krajina::zmena(){
	typ++;
	if( typ == 3)
		typ = 0;




}

