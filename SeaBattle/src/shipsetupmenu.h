#ifndef SHIPSETUPMENU_H
#define SHIPSETUPMENU_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include "common.h"

class ShipSetupMenu : public QWidget
{
    Q_OBJECT
private:
    QGroupBox* shipsGB;
    // кнопки довления кораблей на поле
    QPushButton* addShip1;
    QPushButton* addShip2;
    QPushButton* addShip3;
    QPushButton* addShip4;

    // лейблы для отображения количества кораблей на поле
    QLabel* ship1Label;
    QLabel* ship2Label;
    QLabel* ship3Label;
    QLabel* ship4Label;

    // переменные хранящие число установленных на поле кораблей каждого типа
    int ship1OnField;
    int ship2OnField;
    int ship3OnField;
    int ship4OnField;

    QPushButton* deleteShip; // кнопка "Удалить"
    QPushButton* submit; // кнопка "Играть"
    QPushButton* back; // кнопка "Назад"
    QPushButton* rotate; // кнопка "Повернуть"
    QPushButton* random; // кнопка "Случайно"

    QLabel* statusLabel; // лейбл для отбражения инфрмации о том чей ход

    QLineEdit* ipLineEdit; // строка ввода IP адреса
    QLineEdit* portLineEdit; // строка ввода порта подключения
    QLabel* ipLabel;
    QLabel* portLabel;

    int activeShipSize; // размер активного корабля
    bool conflict; // флаг конфликта расстановки на поле
    bool isServer; // является ли запущенный экземпляр программы сервером

    QString* ipAdress; // IP адрес
    int* port; // порт

public:
    explicit ShipSetupMenu(QString* _ipAdress, int* _port, QWidget *parent = 0);
    void setServer(bool val); // сеттер для isServer

signals:
    void backClicked(); // сигнал нажатия кнопки "Назад"
    void rotateClicked(); // сигнал нажатия кнопки "Повернуть"
    void addShipClicked(int size); // сигнал нажатия кнопок "Добавить"
    void deleteShipFromField(); // сигнал нажатия кнопки "Удалить"
    void generateRandom(); // сигнал нажатия кнопки "Случайно"
    void submitClicked(); // сигнал нажатия кнопки "Играть"

public slots:
    void shipActivated(bool val, int size); // слот обрабатывающий активацию/деактивацию корабля в режиме расстановки
    // слоты обрабатывающие нажатия на кнопки добавления кораблей
    void addShip1Clicked();
    void addShip2Clicked();
    void addShip3Clicked();
    void addShip4Clicked();
    void shipConflict(bool val); // слот обрабатывающий появление или разрешение конфликта расстановки на поле
    void deleteShipClicked(); // слот обрабатывающий нажатие на кнопку добавления корабля
    void reset(); // сброс значений класса
    void generateRandomClicked(); // слот обрабатывающий нажатие на кнопку "Случайно"
    void updateSubmitState(); // слот проверяющий расставлены ли все корабли на поле, если да то делает кнопку "Играть" активной
    void setStatusLabelText(QString text);
    void ipEdited(); // слот обрабатывающий окончание реадактирования в строке IP адреса
    void portEdited(); // слот обрабатывающий окончание реадактирования в строке порта
};

#endif // SHIPSETUPMENU_H
