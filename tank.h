#ifndef TANK_H
#define TANK_H


class Tank
{
public:
	float x, y, uhel, rk, o, r, g, b, prach;
	Tank();
	void kresli();
	void vlevo();
	void vpravo();
	void umisti(int xu, int yu);
	void naloz();
};

#endif // TANK_H
