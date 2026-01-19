#ifndef BOMBA_H
#define BOMBA_H
#include "strela.h"

class bomba:public strela
{
public:
	bomba(float uhel, float prach, float x, float y);

	void kresli();
	virtual bool pohni(Krajina *k);
};

#endif // BOMBA_H
