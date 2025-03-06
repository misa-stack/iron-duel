#include "vybuch.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

vybuch::vybuch(int x, int y): xk(x), yk(y)
{

	rk = 1;
	zvetsovanikonec= false;
}

void vybuch::kresli()
{
<<<<<<< HEAD
    barva(237,98,64);
    kruh(xk,yk,rk);
=======
			barva(237,98,64);
	kruh(xk,yk,rk);
>>>>>>> e274eb559e42b4ae6365523694c6ba6b62aa803c


	if (rk < 30 & rk > 0) {         //velikost výbuchu

		if (zvetsovanikonec == false){
			if (rk < 29 & rk > 0) {
				rk = rk + 1 ;       //zvětšování
				if (rk == 28)
					zvetsovanikonec = true;
			}}

		if (zvetsovanikonec == true){
			{if (rk < 30 & rk > 0) {

					rk = rk -1 ;
				}
				else{

					rk = 0;         //kdyby náhodou výpočet šel mimo interval tak se to vynuluje
				}}}
	}

<<<<<<< HEAD
                    rk = 0;         //kdyby náhodou výpočet šel mimo interval tak se to vynuluje
                }}}
    }
=======
>>>>>>> e274eb559e42b4ae6365523694c6ba6b62aa803c
	barva(237,98,64);
	kruh(xk,yk,rk);
	barva(237,98,64);
	kruh(xk,yk,rk);
	if (zvetsovanikonec == false){
		if (rk < 29 & rk > 0) {
			rk = rk + 1 ;
			if (rk == 28)
				zvetsovanikonec = true;
		}}

	if (zvetsovanikonec == true){
		{if (rk < 30 & rk > 0) {
				rk = rk -1 ;
			}
			else{

				rk = 0;
			}}}
}

