#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QtNetwork>
#include "battlefield.h"
#include "gamemenu.h"
#include "shipsetupmenu.h"
#include "logmenu.h"
#include "battlestat.h"

namespace Ui {
class MainWindow;
}

// класс главного окна

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QHBoxLayout* centralWidgetLayout;
    BattleField* myBF; // мое игровое поле
    BattleField* enemyBF; // игоровое поле противника
    GameMenu* gMenu; // игровое меню
    ShipSetupMenu* ssMenu; // меню расстановки кораблей
    LogMenu* lMenu; // меню с игровым логом
    QLabel* myShips;
    QLabel* enemyShips;

    char abc[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
    bool isServer; // является ли запущенный экземпляр программы сервером
    bool clientReady; // флаг готовности сервера к игре
    bool serverReady; // флаг готовности клиента к игре
    GAME_STATUS gstatus; // игровой статус
    BattleStat batStats; // игровая статистика
    bool gameover; // GAME OVER

    //сеть
    QString ipAdress; // IP адрес подключения
    int port; // порт подключения
    QTcpServer *tcpServer; // TCP сервер
    QTcpSocket *tcpSocket; // TCP порт
    quint16 blockSize; // размер в байтах передаваемого пакета

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:
    void createGame(); // создем новую игру
    void connectToGame(); // подключаемся к существующей игре
    void backToGameMenu(); // возвращаемся в игровое меню
    void startGame(); // переход от редатактора к началу игры
    void tryToStartGame(); // сервер проверяет готовность обоих игроков к игре, если оба готовы - начинает игру
    void gameOver(PLAYERS pl); // конец игры

    //сеть
    void startServer(); // старт TCP сервера
    void stopServer(); // остановка TCP сервера
    void startClient(); // старт TCP клиента
    void stopClient(); // остановка TCP клиента
    bool connectToServer(); // подключение к серверу
    void acceptConnection(); // принятие соединения сервером
    void readDataFromSocket(); // чтение данных из сокета
    void sendTurn(int x, int y); // передача хода противнику для проверки
    void sendTurnResponse(int x, int y, CELL_STATUS st, Ship* s); // передача ответа на ход
    void sendReadyToServer(); // отправление клиентом статуса готовности на сервер
    void sendGameStatus(GAME_STATUS st); // отправка очередности хода(сервер решает в начале кто ходит первым и уведомдяет об этом клинт)
    void displayError(QAbstractSocket::SocketError socketError); // вывод сообщения об ошибке с сетью
};

#endif // MAINWINDOW_H
