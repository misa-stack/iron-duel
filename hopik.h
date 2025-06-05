#ifndef HOPIK_H
#define HOPIK_H
#include "strela.h"

class hopik:public strela
{
public:
	hopik(float uhel, float prach, float x, float y);
	virtual bool pohni(Krajina *k);
	int pocet = 0;
};

#endif // HOPIK_H
