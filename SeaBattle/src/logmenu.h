#ifndef LOGMENU_H
#define LOGMENU_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>
#include <QGroupBox>
#include "battlestat.h"
#include "common.h"

// класс меню с логом, статусом и статистикой игры

class LogMenu : public QWidget
{
    Q_OBJECT
private:
    QTextBrowser* logBrouser; // текстовый браузер в котором отображается лог боя
    GAME_STATUS* gstatus; // указатель на игровой статус
    BattleStat* batStats; // указатель на объект класса игровой статистики
    QLabel* statusLabel;
    QGroupBox* myShipsGB;
    QGroupBox* enemyShipsGB;
    QLabel* myShipsLabel;
    QLabel* enemyShipsLabel;

public:
    explicit LogMenu(GAME_STATUS* _gstatus, BattleStat* _batStats, QWidget *parent = 0);

signals:

public slots:
    void addLogString(QString text); // добавление строки в лог
    void updateStatusLabel(); // обновление статуса игры и статистики

};

#endif // LOGMENU_H
