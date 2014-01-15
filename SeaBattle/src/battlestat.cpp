#include "battlestat.h"

// конструктор
BattleStat::BattleStat(QObject *parent) :
    QObject(parent)
{
    // инициализируем количества кораблей кажого типа для себя и противника
    myShips[0] = SHIP1_AMOUNT;
    myShips[1] = SHIP2_AMOUNT;
    myShips[2] = SHIP3_AMOUNT;
    myShips[3] = SHIP4_AMOUNT;

    enemyShips[0] = SHIP1_AMOUNT;
    enemyShips[1] = SHIP2_AMOUNT;
    enemyShips[2] = SHIP3_AMOUNT;
    enemyShips[3] = SHIP4_AMOUNT;
}

// удаление корабля в статистике
void BattleStat::shipKilled(PLAYERS pl, int size)
{
    switch(pl)
    {
    case ME: // свои корабли
        myShips[size - 1]--;
        break;
    case ENEMY: // корабли противника
        enemyShips[size - 1]--;
        break;
    }
}

// полчуение статистики по кораблям размера size
int BattleStat::getShipsAmount(PLAYERS pl, int size)
{
    switch(pl)
    {
    case ME: // свои корабли
        return myShips[size - 1];
    case ENEMY: // корабли противника
        return enemyShips[size - 1];
    }
}

// проверка окончания игры
void BattleStat::checkBattleStatus()
{
    // если все свои корабли уничтожены, то посылаем сигнал о том что побел противник
    if(myShips[0] == 0 && myShips[1] == 0 && myShips[2] == 0 && myShips[3] == 0)
        emit gameOver(ENEMY);
    // если все корабли противника уничтожены, то посылаем сигнал о том что мы победили
    if(enemyShips[0] == 0 && enemyShips[1] == 0 && enemyShips[2] == 0 && enemyShips[3] == 0)
        emit gameOver(ME);
}
