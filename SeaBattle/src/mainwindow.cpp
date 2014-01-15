#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QAbstractSocket>
#include <QtMultimedia/QSound>

// конструктор
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    blockSize = 0; // обнуляем размер передаваемого по TCP пакета

    gstatus = NONE; // обнуляем игровой статус
    serverReady = false; // сервер не готов игре
    clientReady = false; // клиент не готов к игре
    gameover = false; // игра не закончена

    // компоновка главного окна
    centralWidgetLayout = new QHBoxLayout(ui->centralWidget);
    QVBoxLayout* fieldsLayout = new QVBoxLayout();
    myBF = new BattleField(&gstatus, &batStats);
    enemyBF = new BattleField(&gstatus, &batStats);
    enemyShips = new QLabel(tr("Поле противника:"));
    fieldsLayout->addWidget(enemyShips);
    fieldsLayout->addWidget(enemyBF);
    myShips = new QLabel(tr("Мое поле:"));
    fieldsLayout->addWidget(myShips);
    fieldsLayout->addWidget(myBF);
    fieldsLayout->addStretch();
    centralWidgetLayout->addLayout(fieldsLayout);
    gMenu = new GameMenu();
    centralWidgetLayout->addWidget(gMenu);
    ssMenu = new ShipSetupMenu(&ipAdress, &port);
    centralWidgetLayout->addWidget(ssMenu);
    lMenu = new LogMenu(&gstatus, &batStats);
    centralWidgetLayout->addWidget(lMenu);
    ssMenu->hide();
    lMenu->hide();
    enemyBF->hide();
    enemyShips->hide();
    myBF->hide();
    myShips->hide();
    this->adjustSize();

    // соединяем сигналы и слоты
    connect(gMenu, SIGNAL(createGameClicked()), this, SLOT(createGame()));
    connect(gMenu, SIGNAL(connectToGameClicked()), this, SLOT(connectToGame()));
    connect(ssMenu, SIGNAL(backClicked()), this, SLOT(backToGameMenu()));
    connect(myBF, SIGNAL(shipActivated(bool, int)), ssMenu, SLOT(shipActivated(bool, int)));
    connect(ssMenu, SIGNAL(rotateClicked()), myBF, SLOT(rotateActiveShip()));
    connect(ssMenu, SIGNAL(addShipClicked(int)), myBF, SLOT(addShipToField(int)));
    connect(myBF, SIGNAL(shipConflict(bool)), ssMenu, SLOT(shipConflict(bool)));
    connect(ssMenu, SIGNAL(deleteShipFromField()), myBF, SLOT(deleteShipFromField()));
    connect(ssMenu,  SIGNAL(generateRandom()), myBF, SLOT(generateRandom()));
    connect(ssMenu, SIGNAL(submitClicked()), this, SLOT(startGame()));
    connect(enemyBF, SIGNAL(cellClicked(int,int)), this, SLOT(sendTurn(int,int)));
    connect(myBF, SIGNAL(turnResponse(int,int,CELL_STATUS,Ship*)), this, SLOT(sendTurnResponse(int,int,CELL_STATUS,Ship*)));
    connect(enemyBF, SIGNAL(updateStatusLabel()), lMenu, SLOT(updateStatusLabel()));
    connect(myBF, SIGNAL(updateStatusLabel()), lMenu, SLOT(updateStatusLabel()));
    connect(&batStats, SIGNAL(gameOver(PLAYERS)), this, SLOT(gameOver(PLAYERS)));
}

// деструктор
MainWindow::~MainWindow()
{
    delete ui;
}

// создем новую игру
void MainWindow::createGame()
{
    isServer = true; // приложение запущено в качестве сервера
    myBF->show(); // отображаем свое поле
    myBF->setSetupMode(true); // устанавливаем свое поле в режим расстановки кораблей
    myShips->show(); // отображаем лейбл "Мое поле:"
    // уведомляем меню расстановки о том что приложение запущено как сервер
    //(скрываются строки ввода пароля и порта)
    ssMenu->setServer(true);
    ssMenu->show(); // отображаем меню расстановки
    gMenu->hide(); // прячем игровое меню
    startServer(); // запускаем TCP сервер
    QApplication::processEvents(); // обрабатываем имеющиеся события
    this->adjustSize(); // подгоняем размер окна
}

// подключаемся к существующей игре
void MainWindow::connectToGame()
{
    isServer = false; // приложение запущено как клиент
    myBF->show(); // отображаем свое поле
    myBF->setSetupMode(true); // устанавливаем свое поле в режим расстановки кораблей
    myShips->show(); // отображаем лейбл "Мое поле:"
    // уведомляем меню расстановки о том что приложение запущено как клиент
    //(отображаются строки ввода пароля и порта)
    ssMenu->setServer(false);
    ssMenu->show(); // отображаем меню расстановки
    gMenu->hide(); // прячем игровое меню
    startClient(); // запускаем TCP клиент
    QApplication::processEvents(); // обрабатываем имеющиеся события
    this->adjustSize(); // подгоняем размер окна
}

// возвращаемся в игровое меню
void MainWindow::backToGameMenu()
{
    myBF->hide(); // прячем свое поле
    myShips->hide(); // прячем лейбл "Мое поле:"
    ssMenu->hide(); // прячем меню расстановки
    gMenu->show(); // отображаем игровое меню
    myBF->reset(); // сбрасиваем расстановку на своем поле
    ssMenu->reset(); // сбрасываем значения расстановленных кораблей в меню расстановки
    if(isServer)
        stopServer(); // остановливаем сервер
    QApplication::processEvents(); // обрабатываем имеющиеся события
    this->adjustSize();  // подгоняем размер окна
}

// переход от редатактора к началу игры
void MainWindow::startGame()
{
    if(!isServer) // если приложение запущено как клиент
    {
        if(!connectToServer()) // пытаемся подключиться к серверу
            return;
        // клиент готов к игре
        gstatus = READY;
        clientReady = true;
        sendReadyToServer(); // посылаем сигнал готовности на сервер
    }
    else
    {
        // сервер готов
        gstatus = READY;
        serverReady = true;
        tryToStartGame(); // сервер проверяет готовы ли игроки, если да то начинается игра
    }
    lMenu->updateStatusLabel(); // обновляем лейбл со статусом игры
    myBF->setSetupMode(false); // выключаем для своего поля режим расстановки
    myBF->setNoActive(); // деактивируем активный корабль если он был
    enemyBF->setBattleMode(true); // устанавливаем поле противника в режим боя
    lMenu->show(); // отображаем меню с логом
    ssMenu->hide(); // прячем меню расстановки
    enemyBF->show(); // отображаем поле противника
    enemyShips->show(); // отображаем лейбл "Поле противника:"
    QApplication::processEvents(); // обрабатываем имеющиеся события
    this->adjustSize(); // подгоняем размер окна
    this->move(40, 0); // пердвигаем немного окно чтобы оно лучше влезало в экран...
}

// старт TCP сервера
void MainWindow::startServer()
{
    tcpServer = new QTcpServer(this);
#ifdef FIXED_PORT
    if (!tcpServer->listen(QHostAddress::Any, PORT)) { // слушаем на всех сетевых интерфейсах на порте PORT
#else
    if (!tcpServer->listen()) { // слушаем на всех сетевых интерфейсах, порт выбирается автоматически
#endif
    QMessageBox::critical(this, tr("Морской бой"),
                          tr("Неудается запустить сервер: %1.")
                          .arg(tcpServer->errorString()));
        return;
    }
    ssMenu->setStatusLabelText(tr("Сервер запущен\n\nпорт: %1\n").arg(tcpServer->serverPort()));

    // соединяем сигнал о том, что имеется новое соединение, со слотом который принимает соединение
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

// остановка TCP сервера
void MainWindow::stopServer()
{
    tcpServer->close();
    delete tcpServer;
}

// старт TCP клиента
void MainWindow::startClient()
{
    tcpSocket = new QTcpSocket(this);
    // соединяем сигнал говорящий о том, что в сокете есть данные для чтения, со слотом читающим эти данные
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()));
    // соединяем сигнал об ошибке сокета со слотом который выводит сообщения об ошибках
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
}


// остановка TCP клиента
void MainWindow::stopClient()
{

}

// вывод сообщения об ошибке с сетью
void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Морской бой"),
                                 tr("Хост не найден. Проверте имя хоста и порт."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Морской бой"),
                                 tr("Соединение было отклонено(или вышел таймаут). "
                                    "Убедитесь что сервер запущен, "
                                    "проверте имя хоста и порт."));
        break;
    case QAbstractSocket::RemoteHostClosedError:
        if(!gameover)
        {
            QMessageBox::information(this, tr("Морской бой"),
                                 tr("Удалённый хост закрыл соединение."));
            close();
        }
        break;
    default:
        QMessageBox::information(this, tr("Морской бой"),
                                 tr("Ошибка: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}

// чтение данных из сокета
void MainWindow::readDataFromSocket()
{
    QDataStream in(tcpSocket);// инициализируем поток данных
    in.setVersion(QDataStream::Qt_5_1);

    if (blockSize == 0) { // если новый пакет данных
        // если в сокете данных меньше чем размер переменной опрделяющей размер пакета, то выходим(ждем больше данных)
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
        // если данных достаточно то читаем размер пакета
        in >> blockSize;
    }

    // если в сокете не весь пакет, то выходим
    if (tcpSocket->bytesAvailable() < blockSize)
        return;

    // если можем прочитать пакет целиком
    blockSize = 0; // обнуляем размер пакета для чтения следующего пакета
    POCKET_TYPE pt; // тип пакета
    int temp; // временная переменная для чтение из сокета и последующего преобразования в перечисляемый тип
    // читаем тип пакета
    in >> temp;
    pt = (POCKET_TYPE)temp;
    int x, y;
    switch(pt)
    {
        case READY_TO_GAME: // пакет содержит сообщение клиента о готовности к игре
        {
            clientReady = true; // клиент готов
            tryToStartGame(); // сервер проверяет готовы ли игроки, если да то начинается игра
            break;
        }
        case SETUP_TURN: // пакет от сервера с установкой игрового статуса для клиента
        {
            in >> temp;
            gstatus = (GAME_STATUS) temp; // меняем статус хода
            lMenu->updateStatusLabel(); // обновляем меню
            break;
        }
        case TURN: // пакет содержит ход противника
        {
            // читаем координаты клетки
            in >> x;
            in >> y;
            lMenu->addLogString(tr("Ход протвника (%1:%2)").arg(abc[x]).arg(y + 1)); // записываем ход в лог
            myBF->checkTurn(x, y); // проверяем ход
            break;
        }
        case TURN_RESPONSE: // пакет содержит ответ на ход
        {
            CELL_STATUS st; // статус клетки
            QPoint pos; // позиция отрисовки корабля(передается в пакете если корабль был убит)
            int size = 0; // размер корабля(передается в пакете если корабль был убит)
            direction dir = HORIZONTAL; // ориентация корабля (передается в пакете если корабль был убит)
            // читаем координаты клетки
            in >> x;
            in >> y;
            in >> temp;
            st = (CELL_STATUS)temp; // читаем статус клетки
            if(st == KILLED) // если корабль убит
            {
                // читаем данные по кораблю
                in >> pos;
                in >> size;
                in >> temp;
                dir = (direction)  temp;
            }
            // отображаем в лог статус хода
            QString text;
            switch (st)
            {
            case KILLED:
                text = tr("Убит");
                break;
            case DAMAGED:
                text = tr("Подбит");
                break;
            case DOT:
                text = tr("Мимо");
                break;
            }
            lMenu->addLogString(text);
            enemyBF->checkTurnResponse(x, y, st, pos, size, dir); // передаем ответ на ход полю противника
            break;
        }
    }
}

// подключение к серверу
bool MainWindow::connectToServer()
{
    tcpSocket->connectToHost(ipAdress, port);
    return tcpSocket->waitForConnected();
}

// принятие соединения сервером
void MainWindow::acceptConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));
    // соединяем сигнал говорящий о том, что в сокете есть данные для чтения, со слотом читающим эти данные
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()));
    // соединяем сигнал об ошибке сокета со слотом который выводит сообщения об ошибках
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
}

// передача хода противнику для проверки
void MainWindow::sendTurn(int x, int y)
{
    QByteArray block; // байтовый массив
    QDataStream out(&block, QIODevice::WriteOnly); // инициализируем поток байтовым массивом
    POCKET_TYPE pt = TURN; // устанавливаем тип пакета
    out.setVersion(QDataStream::Qt_5_1);
    out << (quint16)0; // оставлем место под размер пакета
    out << pt; // записываем тип пакета
    // записываем координаты клетки
    out << x;
    out << y;
    out.device()->seek(0); // возвращаемя к месту зарезервированному под размер пакета
    out << (quint16)(block.size() - sizeof(quint16)); // записываем размер пакета
    tcpSocket->write(block); // пишем массив в сокет
    lMenu->addLogString(tr("Ваш ход (%1:%2)").arg(abc[x]).arg(y + 1)); // добавляем строку о ходе в лог
}

// передача ответа на ход
void MainWindow::sendTurnResponse(int x, int y, CELL_STATUS st, Ship *s)
{
    QByteArray block; // байтовый массив
    QDataStream out(&block, QIODevice::WriteOnly); // инициализируем поток байтовым массивом
    POCKET_TYPE pt = TURN_RESPONSE; // устанавливаем тип пакета
    out.setVersion(QDataStream::Qt_5_1);
    out << (quint16)0; // оставлем место под размер пакета
    out << pt; // записываем тип пакета
    // записываем координаты клетки
    out << x;
    out << y;
    // записываем статус клетки
    out << st;
    if(st == KILLED) // если убит записваем данные по кораблю
    {
        out << s->getPosition(); // тоска отрисовки
        out << s->getSize(); // размер
        out << s->getDirection(); // ориентация
    }
    out.device()->seek(0); // возвращаемя к месту зарезервированному под размер пакета
    out << (quint16)(block.size() - sizeof(quint16)); // записываем размер пакета
    tcpSocket->write(block); // пишем массив в сокет
    // отображаем в лог статус хода
    QString text;
    switch (st)
    {
    case KILLED:
        text = tr("Убит");
        break;
    case DAMAGED:
        text = tr("Подбит");
        break;
    case DOT:
        text = tr("Мимо");
        break;
    }

    lMenu->addLogString(text);
}

 // отправление клиентом статуса готовности на сервер
void MainWindow::sendReadyToServer()
{
    QByteArray block; // байтовый массив
    QDataStream out(&block, QIODevice::WriteOnly); // инициализируем поток байтовым массивом
    POCKET_TYPE pt = READY_TO_GAME; // устанавливаем тип пакета
    out.setVersion(QDataStream::Qt_5_1);
    out << (quint16)0; // оставлем место под размер пакета
    out << pt; // записываем тип пакета
    out.device()->seek(0); // возвращаемя к месту зарезервированному под размер пакета
    out << (quint16)(block.size() - sizeof(quint16)); // записываем размер пакета
    tcpSocket->write(block); // пишем массив в сокет
}


void MainWindow::sendGameStatus(GAME_STATUS st)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    POCKET_TYPE pt = SETUP_TURN;
    out.setVersion(QDataStream::Qt_5_1);
    out << (quint16)0; // оставлем место под размер пакета
    out << pt;  // записываем тип пакета
    out << st; // записываем игровой статус(чей ход)
    out.device()->seek(0); // возвращаемя к месту зарезервированному под размер пакета
    out << (quint16)(block.size() - sizeof(quint16)); // записываем размер пакета
    tcpSocket->write(block); // пишем массив в сокет
}

// сервер проверяет готовность обоих игроков к игре, если оба готовы - начинает игру
void MainWindow::tryToStartGame()
{
    if(serverReady && clientReady) // если клиент и сервер готовы
    {
        if(rand() % 2 == 0) // подбрасываем монетку чтобы решить кто ходит первый
        {
            // мой ход
            gstatus = YOUR_TURN;
            sendGameStatus(ENEMY_TURN); // уведомляем клиент о том чей ход
            lMenu->updateStatusLabel();
        }
        else
        {
            // ход противника
            gstatus = ENEMY_TURN;
            sendGameStatus(YOUR_TURN); // уведомляем клиент о том чей ход
            lMenu->updateStatusLabel();
        }
    }
}

// конец игры
void MainWindow::gameOver(PLAYERS pl)
{
    gameover = true;
    QString text;
    QString sound;
    switch(pl)
    {
    case ME: // победил я
        text = tr("Вы победили! =)");
        sound = ":/sounds/Sounds/sdwin.wav";
        break;
    case ENEMY: // победил противник
        sound = ":/sounds/Sounds/nave.wav";
        text = tr("Вы проиграли! =(");
    }
    QSound::play(sound); // звук
    QMessageBox::information(this, tr("Морской бой"), text); // вывод сообщения о победе/поражениии
    if(isServer)
    {
        tcpServer->close();
    }
    close();
}


