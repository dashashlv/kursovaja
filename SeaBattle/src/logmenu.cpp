#include "logmenu.h"

LogMenu::LogMenu(GAME_STATUS *_gstatus, BattleStat *_batStats, QWidget *parent) : // конструктор
    QWidget(parent)
{
    gstatus = _gstatus; // присваиваем указатель на переменную статуса игры из MainWindow
    batStats = _batStats; // присваиваем указатель на объект класса игровой статистики из MainWindow
    // компановка виджета
    QVBoxLayout* layout = new QVBoxLayout(this);
    logBrouser = new QTextBrowser();
    QLabel* logLabel = new QLabel(tr("Игровой лог:"));
    layout->addWidget(logLabel);
    layout->addWidget(logBrouser);
    statusLabel = new QLabel();
    layout->addWidget(statusLabel);
    enemyShipsGB = new QGroupBox(tr("Корабли противника:"));
    QVBoxLayout* eShipsGBLayout = new QVBoxLayout();
    enemyShipsLabel = new QLabel();
    eShipsGBLayout->addWidget(enemyShipsLabel);
    enemyShipsGB->setLayout(eShipsGBLayout);
    myShipsGB = new QGroupBox(tr("Мои корабли:"));
    QVBoxLayout* mShipsGBLayout = new QVBoxLayout();
    myShipsLabel = new QLabel();
    mShipsGBLayout->addWidget(myShipsLabel);
    myShipsGB->setLayout(mShipsGBLayout);
    layout->addWidget(enemyShipsGB);
    layout->addWidget(myShipsGB);
    updateStatusLabel();
}

// добавление строки в лог
void LogMenu::addLogString(QString text)
{
    logBrouser->append(text);
}

// обновление статуса игры и статистики
void LogMenu::updateStatusLabel()
{
    QFont font;
    font.setBold(true);
    font.setPixelSize(15);
    statusLabel->setFont(font);
    statusLabel->setAlignment(Qt::AlignCenter);
    QPalette palette = statusLabel->palette();
    switch (*gstatus)
    {
    case READY:
        palette.setColor(QPalette::Disabled, QPalette::Foreground, Qt::darkYellow);
        palette.setColor(QPalette::Active, QPalette::Foreground, Qt::darkYellow);
        palette.setColor(QPalette::Inactive, QPalette::Foreground, Qt::darkYellow);
        statusLabel->setPalette(palette);
        statusLabel->setText("ЖДЕМ ПРОТИВНИКА");
        break;
    case YOUR_TURN:
        palette.setColor(QPalette::Normal, QPalette::Foreground, Qt::blue);
        palette.setColor(QPalette::Active, QPalette::Foreground, Qt::blue);
        palette.setColor(QPalette::Inactive, QPalette::Foreground, Qt::blue);
        statusLabel->setPalette(palette);
        statusLabel->setText(tr("ВАШ ХОД"));
        break;
    case ENEMY_TURN:
        palette.setColor(QPalette::Normal, QPalette::Foreground, Qt::red);
        palette.setColor(QPalette::Active, QPalette::Foreground, Qt::red);
        palette.setColor(QPalette::Inactive, QPalette::Foreground, Qt::red);
        statusLabel->setPalette(palette);
        statusLabel->setText(tr("ХОД ПРОТИВНИКА"));
        break;
    }
    enemyShipsLabel->setText(tr("Однопалубных:\t\t %1/%2\n"
                             "Двухпалубных:\t\t %3/%4\n"
                             "Трехпалубных:\t\t %5/%6\n"
                             "Четырехпалубных:\t %7/%8\n")
                          .arg(batStats->getShipsAmount(ENEMY, SHIP1))
                          .arg(SHIP1_AMOUNT)
                          .arg(batStats->getShipsAmount(ENEMY, SHIP2))
                          .arg(SHIP2_AMOUNT)
                          .arg(batStats->getShipsAmount(ENEMY, SHIP3))
                          .arg(SHIP3_AMOUNT)
                          .arg(batStats->getShipsAmount(ENEMY, SHIP4))
                          .arg(SHIP4_AMOUNT));
    myShipsLabel->setText(tr("Однопалубных:\t\t %1/%2\n"
                             "Двухпалубных:\t\t %3/%4\n"
                             "Трехпалубных:\t\t %5/%6\n"
                             "Четырехпалубных:\t %7/%8\n")
                          .arg(batStats->getShipsAmount(ME, SHIP1))
                          .arg(SHIP1_AMOUNT)
                          .arg(batStats->getShipsAmount(ME, SHIP2))
                          .arg(SHIP2_AMOUNT)
                          .arg(batStats->getShipsAmount(ME, SHIP3))
                          .arg(SHIP3_AMOUNT)
                          .arg(batStats->getShipsAmount(ME, SHIP4))
                          .arg(SHIP4_AMOUNT));
}
