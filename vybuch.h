#ifndef VYBUCH_H
#define VYBUCH_H
#include "krajina.h"


class vybuch
{
public:
	vybuch(int x, int y, int ve);
	int rk;
	int xk;
	int yk;
	int velikost;
	bool zvetsovanikonec;
	bool kresli(Krajina &k);
};

#endif // VYBUCH_H
