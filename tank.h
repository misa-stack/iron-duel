#ifndef TANK_H
#define TANK_H


class Tank
{
public:
	float x, y, uhel, rk, o, r, g, b, prach;
	int zivoty = 100;
	Tank();
	void kresli(bool aktivni);
	void vlevo();
	void vpravo();
	void umisti(int xu, int yu);
	void naloz();
};

#endif // TANK_H
