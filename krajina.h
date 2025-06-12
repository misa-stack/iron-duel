#ifndef KRAJINA_H
#define KRAJINA_H


class Krajina
{
public:
	Krajina();
	void kresli();
	void mojecara(int x1, int y1, int x2, int y2);
	void kopec(int x1, int y1, int x2, int y2);
	void hory(int x1, int y1, int x2, int y2);
	void poust(int x1, int y1, int x2, int y2);
	void zmena();
	bool jeHlina(int x, int y);
	int kdeJeHlina(int x);
	void vyzobni(int x, int y);
	bool padej();
	int typ;
	int mapa [800][600];
};

#endif // KRAJINA_H
