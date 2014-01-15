#ifndef GAMEMENU_H
#define GAMEMENU_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

// класс игрового меню
class GameMenu : public QWidget
{
    Q_OBJECT
private:
    QPushButton* createGame; // кнопка "Создать игру"
    QPushButton* connectToGame; // кнопка "Присоединиться к игре"
    QLabel* gameLabel;

public:
    explicit GameMenu(QWidget *parent = 0);

signals:
    void createGameClicked();
    void connectToGameClicked();

public slots:

};

#endif // GAMEMENU_H
