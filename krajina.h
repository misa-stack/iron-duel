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
    int typ;
};

#endif // KRAJINA_H
