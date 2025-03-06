#include "vybuch.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

vybuch::vybuch(int x, int y): xk(x), yk(y)
{
<<<<<<< HEAD
rk = 1;
	rk = 1;

	rk = 29;
=======
	rk = 1;
	zvetsovanikonec= false;
>>>>>>> 81aab960865b3e21f68fad4e4b6cfc571fcf571a
}

void vybuch::kresli()
{
	barva(237,98,64);
	kruh(xk,yk,rk);
    barva(237,98,64);
    kruh(xk,yk,rk);
<<<<<<< HEAD


	if (rk < 30 & rk > 0) {
=======
    if (zvetsovanikonec == false){
	 if (rk < 29 & rk > 0) {
		rk = rk + 1 ;
		if (rk == 28)
			zvetsovanikonec = true;
	}}

    if (zvetsovanikonec == true){
    {if (rk < 30 & rk > 0) {
>>>>>>> 81aab960865b3e21f68fad4e4b6cfc571fcf571a
		rk = rk -1 ;
	}
	else{

		rk = 0;
	}}}
}

