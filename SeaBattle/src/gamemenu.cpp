#include "gamemenu.h"

GameMenu::GameMenu(QWidget *parent) :
    QWidget(parent)
{
    // компановка виджета
    QVBoxLayout* layout = new QVBoxLayout(this); // главный компоновщик
    QHBoxLayout* nameLayout = new QHBoxLayout(); // компоновщик для названия игры
    gameLabel = new QLabel(tr("*МОРСКОЙ БОЙ*")); // лейбл с названием игры
    QFont font; // шрифт
    font.setBold(true); // делаем шрифт "жирным"
    font.setPixelSize(25); // устаналиваем размер шрифта
    gameLabel->setFont(font); // устанвливаем шрифт для лейбла
    createGame = new QPushButton(tr("Создать игру")); // создаем кнопку
    connectToGame = new QPushButton(tr("Присоединиться")); // создаем кнопку
    nameLayout->addStretch();
    nameLayout->addWidget(gameLabel); // добавляем в компоновщик имени лейбл с названием игры
    nameLayout->addStretch();
    layout->addLayout(nameLayout); // добавляем в главный компоновщик компоновщик с названием игры
    // добавляем в главный компоновщик кнопки
    layout->addWidget(createGame);
    layout->addWidget(connectToGame);
    layout->addStretch();

    // соединянем слоты и сигналы
    connect(createGame, SIGNAL(clicked()), this, SIGNAL(createGameClicked()));
    connect(connectToGame, SIGNAL(clicked()), this, SIGNAL(connectToGameClicked()));
}
