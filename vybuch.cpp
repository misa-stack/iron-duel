#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "vybuch.h"
#include <math.h>
vybuch::vybuch(int x, int y): xk(x), yk(y)
{

	rk = 1;
	zvetsovanikonec= false;
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
			barva(255, sqrt(v*v+z*z)/rk*250, 0);
			bod(xk+z,yk+v);
			bod(xk-z,yk+v);
		}


	}


	if (rk < 30 && rk > 0)          //velikost výbuchu
	{
		if (zvetsovanikonec == false){
			if (rk < 29 && rk > 0) {
				rk = rk + 1 ;       //zvětšování
				if (rk == 28)
					zvetsovanikonec = true;
			}
		}

		if (zvetsovanikonec == true){

			if (rk < 30 && rk > 0) {
				rk = rk -1 ;
				if(rk == 0)
					return true;
			}

		}
	}
	return false;
}





