#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
#include "vybuch.h"
vybuch::vybuch(int x, int y): xk(x), yk(y)
{

    rk = 1;
    zvetsovanikonec= false;
}

bool vybuch::kresli(Krajina &k)
{
    barva(237,98,64);
    kruh(xk,yk,rk);

    //vyzobava kruh do pole mapa, ze ktereho se pak kresli obrazovka
    for(int z = 0;z <rk;z++){

        for(int v = yk-sqrt(rk*rk-z*z); v < yk+sqrt(rk*rk-z*z); v++)
        {
            k.vyzobni(xk+z,v);
            k.vyzobni(xk-z,v);
        }


    }


    if (rk < 30 && rk > 0) {         //velikost výbuchu

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





