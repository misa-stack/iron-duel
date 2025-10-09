#ifndef ULTIMATNIZBRAN_H
#define ULTIMATNIZBRAN_H
#include "strela.h"

class ultimatnizbran:public strela
{
public:
        ultimatnizbran(float uhel, float prach, float x, float y);
        virtual bool pohni(Krajina *k);
        int pocet = 0;
};

#endif // ULTIMATNIZBRAN_H
