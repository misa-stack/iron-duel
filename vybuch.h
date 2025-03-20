#ifndef VYBUCH_H
#define VYBUCH_H
#include "krajina.h"


class vybuch
{
public:
	vybuch(int x, int y);
	int rk;
	int xk;
	int yk;
	bool zvetsovanikonec;
	void kresli(Krajina &k);
};

#endif // VYBUCH_H
