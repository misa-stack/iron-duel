#ifndef RAKETA_H
#define RAKETA_H
#include "strela.h"


class raketa:public strela
{
public:
	raketa(float x,float y);
	float sx;
	float sy;
	float natoceni;
	void kresli();
	void pohni();
};

#endif // RAKETA_H
