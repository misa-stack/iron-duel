#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "vybuch.h"
#include "tank.h"
#include <math.h>

extern std::list<Tank*> tanky;

vybuch::vybuch(int x, int y, int ve): xk(x), yk(y)
{
	rk = 1;
	zvetsovanikonec= false;
	velikost = ve;
}

bool vybuch::kresli(Krajina &k)
{
	//barva(237,98,64);
	//kruh(xk,yk,rk);


	//vyzobava kruh do pole mapa, ze ktereho se pak kresli obrazovka
	for(int z = 0;z <rk;z++){

		for(int v = -sqrt(rk*rk-z*z); v < +sqrt(rk*rk-z*z); v++)
		{
			k.vyzobni(xk+z,yk+v);
			k.vyzobni(xk-z,yk+v);
			barva(255, sqrt(v*v+z*z)/rk*-255, 0);
			bod(xk+z,yk+v);
			bod(xk-z,yk+v);

			for (auto zasazeny: tanky)
				if ((zasazeny->x == xk+z || zasazeny->x == xk-z) && zasazeny->y == yk+v)
					zasazeny->zivoty --;

		}


	}


	if (rk < velikost && rk > 0)          //velikost výbuchu
	{
		if (zvetsovanikonec == false){
			if (rk < velikost-1 && rk > 0) {
				rk = rk + 1 ;       //zvětšování
				if (rk == velikost-2)
					zvetsovanikonec = true;
			}
		}

		if (zvetsovanikonec == true){

			if (rk < velikost && rk > 0) {
				rk = rk -1 ;
				if(rk == 0)
					return true;
			}

		}
	}
	return false;
}





