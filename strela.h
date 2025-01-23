#ifndef STRELA_H
#define STRELA_H


class strela
{
public:
	strela(float x,float y);
	float vy;
	float vx;
	float ay;
	float x;
	float y;
	int s;
	void kresli();
	void pohni();
	void zmena();
};

#endif // STRELA_H
