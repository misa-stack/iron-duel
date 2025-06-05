#ifndef RAKETA_H
#define RAKETA_H
#include "strela.h"


class raketa:public strela
{
public:
	raketa(float uhel, float prach, float x, float y);
	float sx;
	float sy;
	float natoceni;
	void kresli();
	virtual bool pohni(Krajina *k);
};

#endif // RAKETA_H
