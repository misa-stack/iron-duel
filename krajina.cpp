#include "krajina.h"
#include "grafika.h"

#include <time.h>

Krajina::Krajina()
{
	srand(time(NULL));
	typ = rand() % 3;
	if(typ == 0)
	{
        kopec(0, nahoda(400) + 200,800, nahoda(400) + 200);
	}
	if (typ ==1)
	{
        hory(0, nahoda(400) + 200,800, nahoda(400) + 200);
	}
	if (typ == 2)
	{
        poust(0,nahoda(400)+200,800,nahoda(400)+200);
	}
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

//perkresluje vygenerovanou krajinu do 2d pole "mapa", abychom presne vedeli, na kterych bodech je a neni krajina
void Krajina::mojecara(int x1, int y1, int x2, int y2)
{
	float y = y1;
	float k = float(y2-y1)/float(x2-x1);

	for(int x = x1; x < x2; x++)
	{
		for(int filly = 0; filly < 600; filly++)
			mapa [x][filly] = filly >= y ? 1 : 0;
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


	if(typ == 0) barva(65,152,10);
	if(typ == 1) barva(130,130,130);
	if(typ == 2) barva(223,226,127);

	for (int x = 0; x < 799; x ++)
		for (int y = 0; y < 599; y++)
		{
			if (mapa [x][y] == 1)
				bod(x,y);
		}

}

void Krajina::zmena()
{
	typ++;
	srand(time(NULL));
	if ( typ == 3)
	{
		typ = 0;
	}
	if(typ == 0)
	{
		kopec(0, nahoda(400) + 200,799, nahoda(400) + 200);
	}
	if (typ ==1)
	{
		hory(0, nahoda(400) + 200,799, nahoda(400) + 200);
	}
	if (typ == 2)
	{
		poust(0,nahoda(400)+200,799,nahoda(400)+200);
	}
}



// zjistuje jestli na danem bode je nebo není krajina
bool Krajina::jeHlina(int x, int y)
{
	if(y > 599) return true;

	if(x < 0 && y < 600 || x > 799 && y < 600 || y < 0)
		return false;

	return mapa [x][y] == 1;
}

//zjistuje vysku krajiny na dane x souradnici
int Krajina::kdeJeHlina(int x)
{
	if(x<800)
	{
	for (int y = 0; y < 600; y++)
	{
		if (mapa [x][y] == 1)
			return y;
	}
	}
	return 599;
}

// zmeni dany bod v mape na 0 (tzn. pozadi)
void Krajina::vyzobni(int x, int y)
{
	if(x >= 0 && x < 800 && y >= 0 && y < 600)
		mapa [x][y] = 0;
}

//dela padani prevysle krajiny po vybuchu, "pohnuto" znaci, jestli jeste nejake pixely mohou spadnout nebo ne
bool Krajina::padej()
{
	bool pohnuto = false;
	for (int x = 0; x < 799; x++)
	{
		for (int y = 598; y >= 0; y--)
		{
			if (mapa [x][y] == 1 && mapa [x][y+1] == 0)
			{
				mapa [x][y] = 0;
				mapa [x][y+1] = 1;
				pohnuto = true;
			}
		}
	}
	return !pohnuto;
}





