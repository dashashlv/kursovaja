#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <QWidget>
#include <QPainter>
#include <QList>

#include "ship.h"
#include "common.h"
#include "battlestat.h"

// класс игрового поля

class BattleField : public QWidget
{
    Q_OBJECT
private:
    char abc[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
    int field_size; //размер поля
    CELL_STATUS field[10][10]; // массив состояний клеток поля

    QList<Ship*> ships; // корабли на поле
    Ship* activeShip; // указатель на активный корабль
    Ship* conflictShip; // указатель на конфликтующий корабль

    QPoint lastPoint; // предидущая точка на поле, в которой был вызван mouseMoveEvent
    bool shipMoving; // true если корабль перетаскивается на поле

    bool setupMode; // режим расстановки кораблей
    bool battleMode; // игровой режим

    GAME_STATUS* gstatus; // указатель на текущий игровой статус (READY, YOUR_TURN, ENEMY_TURN)
    BattleStat* batStats; // указатель на игровую статистику
    bool clicked; // флаг клика по полю

public:
    explicit BattleField(GAME_STATUS* _gstatus, BattleStat* _batStats, QWidget *parent = 0); //конструктор
    void setSetupMode(bool mode); // установка значения режима расстановки кораблей
    void setBattleMode(bool mode); // установка значения игрового режима

signals:
    void shipActivated(bool val, int size); // сигнал о выборе пользователем корабля на поле
    void shipConflict(bool val);// сигнал о появлении/разрешении конфликта в расстановке кораблей
    void cellClicked(int x,int y); //сигнял о клике на ячейку игрового поля
    void turnResponse(int x, int y, CELL_STATUS st, Ship* s); // ответ на ход противника
    void updateStatusLabel(); // сигнал о необходимости обновления статуса хода и игровой статситики

public slots:
    void rotateActiveShip(); // поворот корабля
    void addShipToField(int size); // добавление нового корабля на поле
    void deleteShipFromField(); // удаление корабля с поля
    void reset();
    void generateRandom(); // генерация случайной расстановки кораблей
    void drawDot(QPainter* painter,int x,int y); // рисование точки
    void drawDamaged(QPainter* painter, int x, int y); // рисование подбития
    void checkTurn(int x, int y); // проверка хода противника
    void checkTurnResponse(int x, int y, CELL_STATUS st, QPoint pos, int size, direction dir); //проверка ответа противника на ход
    void fillShipDots(QPoint pos, int size, direction dir); // установка точек вокруг убитого корабля
    void setNoActive(); // деактивация выделеного корабля

protected:
    void paintEvent(QPaintEvent *event); // обработчик события отрисовки виджета поля
    void mousePressEvent(QMouseEvent *event); // обработчик нажатия мыши на поле
    void mouseMoveEvent(QMouseEvent *event); // обработчик передвижения курсора по полю
    void mouseReleaseEvent(QMouseEvent *event); // обработчик отпускания кнопки мыши

};

#endif // BATTLEFIELD_H
