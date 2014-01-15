#ifndef COMMON_H
#define COMMON_H

#define CELL 25 // размер клетки поля
#define DOTR CELL / 8 // размер точки
#define FIELD_OFFSET 20 // отступ поля от края виджета

// количества кораблей
#define SHIP1_AMOUNT 4
#define SHIP2_AMOUNT 3
#define SHIP3_AMOUNT 2
#define SHIP4_AMOUNT 1

// размеры кораблей
#define SHIP1 1
#define SHIP2 2
#define SHIP3 3
#define SHIP4 4

// использовать фиксированный порт
#define FIXED_PORT

#define PORT 17000

enum direction{HORIZONTAL, VERTICAL}; // расположение корабля
enum CELL_STATUS{EMPTY, DOT, DAMAGED, KILLED}; // состояния клетки поля
enum POCKET_TYPE{READY_TO_GAME, SETUP_TURN, TURN, TURN_RESPONSE}; // типы передаваемых пакетов
enum GAME_STATUS{NONE, READY, YOUR_TURN, ENEMY_TURN}; // игровой статус
enum PLAYERS{ME, ENEMY}; // игроки

#endif // COMMON_H
