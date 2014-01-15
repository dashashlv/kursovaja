#ifndef SHIP_H
#define SHIP_H

#include "common.h"
#include <QPoint>
#include <QPainter>
#include <QList>

// класс корабля

class Ship
{
private:
    QPoint drawPoint; // точка рисования верхнего левого угла корабля
    int size; // размер
    direction dir; // ориентация HORIZONTAL/VERTICAL
    bool active; // активность корабля
    bool conflicted; // флаг кнофликта расположения данного корабля
    bool killed; // убит/неубит
    int damages; // количество повреждений
    QList<Ship*>* ships; // список указателей на другие корабли(нужен для определения конфликта расположения)
public:
    Ship(QPoint _drawPoint, int _size, direction _dir, QList<Ship*>* _ships); // конструктор
    void draw(QPainter* painter); // функция отрисовки корабля
    void drawShadow(QPainter* painter); // функция отрисовки "тени" корабля при его перетаскивании
    bool onShip(QPoint p); // проверка принадлежит ли точка кораблю
    void setActive(bool a); // установление активности корабля
    void setConflict(bool c); // установление конфликтности корабля
    void move(int deltax, int deltay); // перемещение корабля на deltax и deltay
    void moveTo(int x, int y); // перемещение корабля в точку (x, y)
    QPoint getPosition(); // функция возвращает координаты левого верхнего угла корабля
    int getSize(); // функция возвращает размер корабля
    direction getDirection(); // функция возвращает ориентацию корабля
    void rotate(); // поворот корабля
    bool isIntercepts(); // функция проверки не конфликтует ли данный корабль с уже установленными на поле кораблями
    bool damageShip(); // нанесение повреждения кораблю
};

#endif // SHIP_H
