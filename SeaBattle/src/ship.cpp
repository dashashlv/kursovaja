#include "ship.h"
#include <QPen>
#include <QBrush>
#include <QRect>

// коструктор
Ship::Ship(QPoint _drawPoint, int _size, direction _dir, QList<Ship *> *_ships)
{
    drawPoint = _drawPoint; // точка отрисовки
    size = _size; // размер
    dir = _dir; // ориентация
    ships = _ships; // указатель на список уже расстановленных кораблей
    active = false; // корабль неактивный
    conflicted = false; // корабль не конфликтный
    killed = false; // не убит
    damages = 0; // не поврежден
}

 // функция отрисовки корабля
void Ship::draw(QPainter *painter)
{
    QPen pen; // карандаш(определяет контур фигур)
    QBrush brush; // кисть(определяет заливку фигур)
    if(conflicted) // корабль конфликтный
    {
        drawShadow(painter); // рисуем "тень"
        brush.setColor(Qt::red); // выбираем цвет заливки
    }
    else if(active) // корабль не конфликтный, но активный
    {
        drawShadow(painter); // рисуем "тень"
        brush.setColor(Qt::green); // выбираем цвет заливки
    }
    else
    {
        brush.setColor(Qt::blue); // выбираем цвет заливки
    }
    brush.setStyle(Qt::SolidPattern); // определяем стиль закраски фигур (SolidPattern - сплошная закраска)
    pen.setWidth(2); // устанавливаем толщину карандаша
    if(active)
        pen.setColor(Qt::darkGreen); // выбираем цвет заливки
    else
        pen.setColor(Qt::gray); // выбираем цвет заливки
    painter->setPen(pen); // передаем карандаш рисовальщику
    painter->setBrush(brush); // передаем кисть рисовальщику
    // отрисовываем все "квадраты" корабля в зависимости от его ориентации
    for(int i = 0 ; i < size ; i++)
    {
        switch(dir)
        {
        case HORIZONTAL:
            painter->drawRect(i * CELL + drawPoint.x(), drawPoint.y(), CELL, CELL);
            break;
        case VERTICAL:
            painter->drawRect(drawPoint.x(), i * CELL + drawPoint.y(), CELL, CELL);
            break;
        }
    }
}

// проверка принадлежит ли точка кораблю
bool Ship::onShip(QPoint p)
{
    QRect rect; // прямоугольник покрывающий корабль
    switch(dir)
    {
    case HORIZONTAL:
        rect.setRect(drawPoint.x(), drawPoint.y(), size * CELL, CELL);
        break;
    case VERTICAL:
        rect.setRect(drawPoint.x(), drawPoint.y(), CELL, size * CELL);
        break;
    }
    return rect.contains(p); //проверям содержит ли прямолугольник точку p
}

// установление активности корабля
void Ship::setActive(bool a)
{
    active = a;
}

// перемещение корабля на deltax и deltay
void Ship::move(int deltax, int deltay)
{
    // расчитываем новые координаты корабля
    int x = drawPoint.x() + deltax;
    int y = drawPoint.y() + deltay;
    // проверяем чтобы корабль не вышел за поле
    if(x < FIELD_OFFSET)
        x = FIELD_OFFSET;
    switch(dir)
    {
    case HORIZONTAL:
        if(x + size * CELL > FIELD_OFFSET + 10 * CELL)
            x = FIELD_OFFSET + 10 * CELL - size * CELL;
        break;
    case VERTICAL:
        if(x + CELL > FIELD_OFFSET + 10 * CELL)
            x = FIELD_OFFSET + 10 * CELL - CELL;
    }
    if(y < FIELD_OFFSET)
        y = FIELD_OFFSET;
    switch(dir)
    {
    case HORIZONTAL:
        if(y + CELL > FIELD_OFFSET + 10 * CELL)
            y = FIELD_OFFSET + 10 * CELL - CELL;
        break;
    case VERTICAL:
        if(y + size * CELL > FIELD_OFFSET + 10 * CELL)
            y = FIELD_OFFSET + 10 * CELL - size * CELL;
        break;
    }
    // изменяем координаты корабля
    drawPoint.setX(x);
    drawPoint.setY(y);
}

// перемещение корабля в точку (x, y)
void Ship::moveTo(int x, int y)
{
    drawPoint.setX(x);
    drawPoint.setY(y);
}

// функция возвращает координаты левого верхнего угла корабля
QPoint Ship::getPosition()
{
    return drawPoint;
}

// функция отрисовки "тени" корабля при его перетаскивании
void Ship::drawShadow(QPainter* painter)
{
    QPen pen; // карандаш(определяет контур фигур)
    QBrush brush; // кисть(определяет заливку фигур)
    bool inrercepts = isIntercepts(); // прверяем конфликтует ли расположение данного корабля с отсальными кораблями
    pen.setWidth(0); // устанавливаем ширину карандаша
    if(inrercepts) // если есть наложение кораблей
        brush.setColor(QColor(255, 0, 0, 150)); // устанавливаем цывет кисти
    else
        brush.setColor(QColor(255, 255, 0, 150)); // устанавливаем цывет кисти
    brush.setStyle(Qt::SolidPattern); // определяем стиль закраски фигур (SolidPattern - сплошная закраска)
    painter->setPen(pen); // передаем карандаш рисовальщику
    painter->setBrush(brush); // передаем кисть рисовальщику
    // вычисляем положение "тени"
    int x, y;
    int up,bottom, rigth, left;
    left = FIELD_OFFSET + ((drawPoint.x() - FIELD_OFFSET) / CELL) * CELL;
    rigth = FIELD_OFFSET + ((drawPoint.x() - FIELD_OFFSET) / CELL) * CELL + CELL;
    up = FIELD_OFFSET + ((drawPoint.y() - FIELD_OFFSET) / CELL) * CELL;
    bottom = FIELD_OFFSET + ((drawPoint.y() - FIELD_OFFSET) / CELL) * CELL + CELL;
    if(drawPoint.x() - left <= CELL / 2)
        x = left;
    else
        x = rigth;
    if(drawPoint.y() - up <= CELL / 2)
        y = up;
    else
        y = bottom;
    // отрисовываем тень
    switch(dir)
    {
        case HORIZONTAL: // горизонтально ориентированные корабли
        {
            painter->drawRect(x, y, CELL * size, CELL);
            pen.setWidth(2);
            if(inrercepts)
                pen.setColor(QColor(255, 0, 0, 200));
            else
                pen.setColor(QColor(255, 255, 0, 200));
            brush.setStyle(Qt::NoBrush);
            painter->setBrush(brush);
            painter->setPen(pen);
            int x1 = x - CELL >= FIELD_OFFSET ? x - CELL : FIELD_OFFSET;
            int y1 = y - CELL >= FIELD_OFFSET ? y - CELL : FIELD_OFFSET;
            int x2 = x + (size + 1) * CELL <= FIELD_OFFSET + 10 * CELL ? x + (size + 1) * CELL : FIELD_OFFSET + 10 * CELL;
            int y2 = y + 2 * CELL <= FIELD_OFFSET + 10 * CELL ? y + 2 * CELL : FIELD_OFFSET + 10 * CELL;
            painter->drawLine(x1, y1, x1, y2);
            painter->drawLine(x2, y1, x2, y2);
            painter->drawLine(x1, y1, x2, y1);
            painter->drawLine(x1, y2, x2, y2);
            break;
        }
        case VERTICAL: // вертикально ориентированные корабли
        {
            painter->drawRect(x, y, CELL, CELL * size);
            pen.setWidth(2);
            if(inrercepts)
                pen.setColor(QColor(255, 0, 0, 200));
            else
                pen.setColor(QColor(255, 255, 0, 200));
            brush.setStyle(Qt::NoBrush);
            painter->setBrush(brush);
            painter->setPen(pen);
            int x1 = x - CELL >= FIELD_OFFSET ? x - CELL : FIELD_OFFSET;
            int y1 = y - CELL >= FIELD_OFFSET ? y - CELL : FIELD_OFFSET;
            int x2 = x + 2 * CELL <= FIELD_OFFSET + 10 * CELL ? x + 2 * CELL : FIELD_OFFSET + 10 * CELL;
            int y2 = y + (size + 1) * CELL <= FIELD_OFFSET + 10 * CELL ? y + (size + 1) * CELL : FIELD_OFFSET + 10 * CELL;
            painter->drawLine(x1, y1, x1, y2);
            painter->drawLine(x2, y1, x2, y2);
            painter->drawLine(x1, y1, x2, y1);
            painter->drawLine(x1, y2, x2, y2);
            break;
        }
    }
}

 // поворот корабля
void Ship::rotate()
{
    // меняем ориентацию корабля
    if(dir == HORIZONTAL)
        dir = VERTICAL;
    else
        dir = HORIZONTAL;
    // корректируем положение корабля чтобы он не вышел за приделы поля
    switch (dir)
    {
    case HORIZONTAL:
        if(drawPoint.x() + size * CELL > FIELD_OFFSET + 10 * CELL)
            drawPoint.setX(FIELD_OFFSET + 10 * CELL - size * CELL);
        if(drawPoint.y() + CELL > FIELD_OFFSET + 10 * CELL)
            drawPoint.setY(FIELD_OFFSET + 10 * CELL -  CELL);
        break;
    case VERTICAL:
        if(drawPoint.y() + size * CELL > FIELD_OFFSET + 10 * CELL)
            drawPoint.setY(FIELD_OFFSET + 10 * CELL - size * CELL);
        if(drawPoint.x() + CELL > FIELD_OFFSET + 10 * CELL)
            drawPoint.setX(FIELD_OFFSET + 10 * CELL -  CELL);
        break;
    }
}

// функция проверки не конфликтует ли данный корабль с уже установленными на поле кораблями
bool Ship::isIntercepts()
{
    bool res = false; // результат, изначально считаем что конфликта нет
    QRect thisRect, candidateRect;
    int x, y;
    int x1, x2, y1, y2;
    int up,bottom, rigth, left;
    left = FIELD_OFFSET + ((drawPoint.x() - FIELD_OFFSET) / CELL) * CELL;
    rigth = FIELD_OFFSET + ((drawPoint.x() - FIELD_OFFSET) / CELL) * CELL + CELL;
    up = FIELD_OFFSET + ((drawPoint.y() - FIELD_OFFSET) / CELL) * CELL;
    bottom = FIELD_OFFSET + ((drawPoint.y() - FIELD_OFFSET) / CELL) * CELL + CELL;
    if(drawPoint.x() - left <= CELL / 2)
        x = left;
    else
        x = rigth;
    if(drawPoint.y() - up <= CELL / 2)
        y = up;
    else
        y = bottom;
    switch(dir)
    {
        case HORIZONTAL:
        {
            int x1 = x - CELL >= FIELD_OFFSET ? x - CELL : FIELD_OFFSET;
            int y1 = y - CELL >= FIELD_OFFSET ? y - CELL : FIELD_OFFSET;
            int x2 = x + (size + 1) * CELL <= FIELD_OFFSET + 10 * CELL ? x + (size + 1) * CELL : FIELD_OFFSET + 10 * CELL;
            int y2 = y + 2 * CELL <= FIELD_OFFSET + 10 * CELL ? y + 2 * CELL : FIELD_OFFSET + 10 * CELL;
            thisRect.setRect(x1, y1, x2 - x1, y2 - y1);
            break;
        }
        case VERTICAL:
        {
            int x1 = x - CELL >= FIELD_OFFSET ? x - CELL : FIELD_OFFSET;
            int y1 = y - CELL >= FIELD_OFFSET ? y - CELL : FIELD_OFFSET;
            int x2 = x + 2 * CELL <= FIELD_OFFSET + 10 * CELL ? x + 2 * CELL : FIELD_OFFSET + 10 * CELL;
            int y2 = y + (size + 1) * CELL <= FIELD_OFFSET + 10 * CELL ? y + (size + 1) * CELL : FIELD_OFFSET + 10 * CELL;
            thisRect.setRect(x1, y1, x2 - x1, y2 - y1);
            break;
        }
    }
    for( int i = 0 ; i < ships->size() ; i++)
    {
        Ship* candidateShip = ships->at(i);
        if(candidateShip != this)
        {
            switch(candidateShip->dir)
            {
                case HORIZONTAL:
                {
                    candidateRect.setRect(candidateShip->drawPoint.x(),
                                          candidateShip->drawPoint.y(),
                                          candidateShip->size * CELL,
                                          CELL);
                    break;
                }
                case VERTICAL:
                {
                candidateRect.setRect(candidateShip->drawPoint.x(),
                                      candidateShip->drawPoint.y(),
                                      CELL,
                                      candidateShip->size * CELL);
                    break;
                }
            }
            if(res = thisRect.intersects(candidateRect))
                return res;
        }
    }
    return res;
}

// установление конфликтности корабля
void Ship::setConflict(bool c)
{
    conflicted = c;
}

// функция возвращает размер корабля
int Ship::getSize()
{
    return size;
}

// нанесение повреждения кораблю
bool Ship::damageShip()
{
    damages++;
    if(damages == size)
        killed = true;
    return killed;
}

// функция возвращает ориентацию корабля
direction Ship::getDirection()
{
    return dir;
}
