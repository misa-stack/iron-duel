#ifndef RAKETA_H
#define RAKETA_H


class raketa
{
public:
	raketa(float x,float y);
	float vy;
	float vx;
	float ay;
	float x;
	float y;
	float sx;
	float sy;
	float natoceni;
	void kresli();
	void pohni();
};

#endif // RAKETA_H
