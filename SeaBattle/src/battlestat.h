#ifndef BATTLESTAT_H
#define BATTLESTAT_H

#include <QObject>
#include "common.h"

// класс статистики кораблей, так же служит для опеределения победителя
class BattleStat : public QObject
{
    Q_OBJECT
private:
    int myShips[4]; // статистика моих кораблей, myShips[i] - количество неубитых кораблей размера i + 1
    int enemyShips[4]; // статистика кораблей противника

public:
    explicit BattleStat(QObject *parent = 0); // конструктор

signals:
    void gameOver(PLAYERS pl); // сигнал об окончании игры

public slots:
    void shipKilled(PLAYERS pl, int size); // удаление корабля в статистике
    int getShipsAmount(PLAYERS pl, int size); // полчуение статистики по кораблям размера size
    void checkBattleStatus(); // проверка окончания игры

};

#endif // BATTLESTAT_H
