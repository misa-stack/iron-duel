#ifndef STRELA_H
#define STRELA_H
#include "krajina.h"


class strela
{
public:
	strela(float uhel, float prach, float x, float y);
	float vy;
	float vx;
	float ay;
	float x;
	float y;
	int vel;
	virtual void kresli();
	virtual bool pohni(Krajina *k);
};

#endif // STRELA_H
