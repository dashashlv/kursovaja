#include "shipsetupmenu.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QFontMetrics>

ShipSetupMenu::ShipSetupMenu(QString* _ipAdress, int* _port,QWidget *parent) :
    QWidget(parent)
{
    ipAdress = _ipAdress;
    port = _port;

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QGridLayout* layout = new QGridLayout();

    shipsGB = new QGroupBox(tr("Добавить корабль"));
    QGridLayout* gbLayout = new QGridLayout(shipsGB);
    addShip1 = new QPushButton(tr("Однопалубный"));
    addShip2 = new QPushButton(tr("Двухпалубный"));
    addShip3 = new QPushButton(tr("Трехпалубный"));
    addShip4 = new QPushButton(tr("Четырехпалубный"));

    ship1OnField = ship2OnField = ship3OnField = ship4OnField = 0;

    ship1Label = new QLabel(QString::number(ship1OnField) + "/" + QString::number(SHIP1_AMOUNT));
    ship2Label = new QLabel(QString::number(ship2OnField) + "/" + QString::number(SHIP2_AMOUNT));
    ship3Label = new QLabel(QString::number(ship3OnField) + "/" + QString::number(SHIP3_AMOUNT));
    ship4Label = new QLabel(QString::number(ship4OnField) + "/" + QString::number(SHIP4_AMOUNT));

    gbLayout->addWidget(addShip1, 0, 0, 1, 1);
    gbLayout->addWidget(ship1Label, 0, 1, 1, 1);
    gbLayout->addWidget(addShip2, 1, 0, 1, 1);
    gbLayout->addWidget(ship2Label, 1, 1, 1, 1);
    gbLayout->addWidget(addShip3, 2, 0, 1, 1);
    gbLayout->addWidget(ship3Label, 2, 1, 1, 1);
    gbLayout->addWidget(addShip4, 3, 0, 1, 1);
    gbLayout->addWidget(ship4Label, 3, 1, 1, 1);

    deleteShip = new QPushButton(tr("Удалить"));
    rotate = new QPushButton(tr("Повернуть"));
    random = new QPushButton(tr("Случайно"));
    submit  = new QPushButton(tr("Играть"));

    layout->addWidget(shipsGB, 0, 0, 1, 2);
    layout->addWidget(deleteShip, 1, 0, 1, 1);
    layout->addWidget(rotate, 1, 1, 1, 1);
    layout->addWidget(random, 2, 0, 1, 1);
    layout->addWidget(submit, 2, 1, 1, 1);

    statusLabel = new QLabel();

    back = new QPushButton(tr("Назад"));

    mainLayout->addLayout(layout);
    mainLayout->addWidget(statusLabel);

    QGridLayout* netLayout = new QGridLayout();
    ipLabel = new QLabel(tr("IP:"));
    ipLineEdit = new QLineEdit();
    QRegExp ipRegExp( "\\b(([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d?\\d|2[0-4]\\d|25[0-5])\\b" );
 //   ipLineEdit->setValidator(new QRegExpValidator(ipRegExp,this));
//    ipLineEdit->setInputMask(QString("000.000.000.000;_"));
    QFontMetrics fm(ipLineEdit->font());
    ipLineEdit->setFixedWidth(fm.size(0, QString("000.000.000.000;_")).width());
    ipLineEdit->setText("127.0.0.1");
    *ipAdress = ipLineEdit->text();
    netLayout->addWidget(ipLabel, 0, 0, 1, 1);
    netLayout->addWidget(ipLineEdit, 0, 1, 1, 1);
    portLabel = new QLabel(tr("Порт:"));
    portLineEdit = new QLineEdit();
    portLineEdit->setFixedWidth(fm.size(0, QString("000.000.000.000;_")).width());
#ifdef FIXED_PORT
    portLineEdit->setText(QString::number(PORT));
#endif
    *port = portLineEdit->text().toInt();
    netLayout->addWidget(portLabel, 1, 0, 1, 1);
    netLayout->addWidget(portLineEdit, 1, 1, 1, 1);

    mainLayout->addLayout(netLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(back);

    deleteShip->setEnabled(false);
    rotate->setEnabled(false);
    submit->setEnabled(false);

    conflict = false;

    connect(back, SIGNAL(clicked()), this, SIGNAL(backClicked()));
    connect(rotate, SIGNAL(clicked()), this, SIGNAL(rotateClicked()));
    connect(addShip1, SIGNAL(clicked()), this, SLOT(addShip1Clicked()));
    connect(addShip2, SIGNAL(clicked()), this, SLOT(addShip2Clicked()));
    connect(addShip3, SIGNAL(clicked()), this, SLOT(addShip3Clicked()));
    connect(addShip4, SIGNAL(clicked()), this, SLOT(addShip4Clicked()));
    connect(deleteShip, SIGNAL(clicked()), this, SLOT(deleteShipClicked()));
    connect(random, SIGNAL(clicked()), this, SLOT(generateRandomClicked()));
    connect(submit, SIGNAL(clicked()), this, SIGNAL(submitClicked()));
    connect(ipLineEdit, SIGNAL(editingFinished()), this, SLOT(ipEdited()));
    connect(portLineEdit, SIGNAL(editingFinished()), this, SLOT(portEdited()));
}

void ShipSetupMenu::shipActivated(bool val, int size)
{
    deleteShip->setEnabled(val);
    rotate->setEnabled(val);
    activeShipSize = size;
}

void ShipSetupMenu::addShip1Clicked()
{
    ship1OnField++;
    ship1Label->setText(QString::number(ship1OnField) + "/" + QString::number(SHIP1_AMOUNT));
    if(ship1OnField == SHIP1_AMOUNT)
        addShip1->setEnabled(false);
    updateSubmitState();
    emit addShipClicked(SHIP1);
}

void ShipSetupMenu::addShip2Clicked()
{
    ship2OnField++;
    ship2Label->setText(QString::number(ship2OnField) + "/" + QString::number(SHIP2_AMOUNT));
    if(ship2OnField == SHIP2_AMOUNT)
        addShip2->setEnabled(false);
    updateSubmitState();
    emit addShipClicked(SHIP2);
}

void ShipSetupMenu::addShip3Clicked()
{
    ship3OnField++;
    ship3Label->setText(QString::number(ship3OnField) + "/" + QString::number(SHIP3_AMOUNT));
    if(ship3OnField == SHIP3_AMOUNT)
        addShip3->setEnabled(false);
    updateSubmitState();
    emit addShipClicked(SHIP3);
}

void ShipSetupMenu::addShip4Clicked()
{
    ship4OnField++;
    ship4Label->setText(QString::number(ship4OnField) + "/" + QString::number(SHIP4_AMOUNT));
    if(ship4OnField == SHIP4_AMOUNT)
        addShip4->setEnabled(false);
    updateSubmitState();
    emit addShipClicked(SHIP4);
}

void ShipSetupMenu::shipConflict(bool val)
{
    conflict = val;
    if(conflict)
    {
        addShip1->setEnabled(false);
        addShip2->setEnabled(false);
        addShip3->setEnabled(false);
        addShip4->setEnabled(false);
    }
    else
    {
        if(ship1OnField < SHIP1_AMOUNT)
            addShip1->setEnabled(true);
        if(ship2OnField < SHIP2_AMOUNT)
            addShip2->setEnabled(true);
        if(ship3OnField < SHIP3_AMOUNT)
            addShip3->setEnabled(true);
        if(ship4OnField < SHIP4_AMOUNT)
            addShip4->setEnabled(true);
    }
    updateSubmitState();
}

void ShipSetupMenu::deleteShipClicked()
{
    switch (activeShipSize)
    {
    case 1:
        ship1OnField--;
        if(ship1OnField < SHIP1_AMOUNT)
            addShip1->setEnabled(true);
        ship1Label->setText(QString::number(ship1OnField) + "/" + QString::number(SHIP1_AMOUNT));
        break;
    case 2:
        ship2OnField--;
        if(ship2OnField < SHIP2_AMOUNT)
            addShip2->setEnabled(true);
        ship2Label->setText(QString::number(ship2OnField) + "/" + QString::number(SHIP2_AMOUNT));
        break;
    case 3:
        ship3OnField--;
        if(ship3OnField < SHIP3_AMOUNT)
            addShip3->setEnabled(true);
        ship3Label->setText(QString::number(ship3OnField) + "/" + QString::number(SHIP3_AMOUNT));
        break;
    case 4:
        ship4OnField--;
        if(ship4OnField < SHIP4_AMOUNT)
            addShip4->setEnabled(true);
        ship4Label->setText(QString::number(ship4OnField) + "/" + QString::number(SHIP4_AMOUNT));
        break;
    }
    updateSubmitState();
    emit deleteShipFromField();
}

void ShipSetupMenu::reset()
{
    ship1OnField = ship2OnField = ship3OnField = ship4OnField = 0;
    addShip1->setEnabled(true);
    addShip2->setEnabled(true);
    addShip3->setEnabled(true);
    addShip4->setEnabled(true);
    ship1Label->setText(QString::number(ship1OnField) + "/" + QString::number(SHIP1_AMOUNT));
    ship2Label->setText(QString::number(ship2OnField) + "/" + QString::number(SHIP2_AMOUNT));
    ship3Label->setText(QString::number(ship3OnField) + "/" + QString::number(SHIP3_AMOUNT));
    ship4Label->setText(QString::number(ship4OnField) + "/" + QString::number(SHIP4_AMOUNT));
    updateSubmitState();
}

void ShipSetupMenu::generateRandomClicked()
{
    reset();
    ship1OnField = SHIP1_AMOUNT;
    ship2OnField = SHIP2_AMOUNT;
    ship3OnField = SHIP3_AMOUNT;
    ship4OnField = SHIP4_AMOUNT;
    addShip1->setEnabled(false);
    addShip2->setEnabled(false);
    addShip3->setEnabled(false);
    addShip4->setEnabled(false);
    ship1Label->setText(QString::number(ship1OnField) + "/" + QString::number(SHIP1_AMOUNT));
    ship2Label->setText(QString::number(ship2OnField) + "/" + QString::number(SHIP2_AMOUNT));
    ship3Label->setText(QString::number(ship3OnField) + "/" + QString::number(SHIP3_AMOUNT));
    ship4Label->setText(QString::number(ship4OnField) + "/" + QString::number(SHIP4_AMOUNT));
    emit generateRandom();
    updateSubmitState();
}

void ShipSetupMenu::updateSubmitState()
{
    if(ship1OnField != SHIP1_AMOUNT)
    {
        submit->setEnabled(false);
        return;
    }
    if(ship2OnField != SHIP2_AMOUNT)
    {
        submit->setEnabled(false);
        return;
    }
    if(ship3OnField != SHIP3_AMOUNT)
    {
        submit->setEnabled(false);
        return;
    }
    if(ship4OnField != SHIP4_AMOUNT)
    {
        submit->setEnabled(false);
        return;
    }
    if(conflict == true)
    {
        submit->setEnabled(false);
        return;
    }
    submit->setEnabled(true);
}

void ShipSetupMenu::setServer(bool val)
{
    isServer = val;
    if(isServer)
    {
        ipLabel->hide();
        ipLineEdit->hide();
        portLabel->hide();
        portLineEdit->hide();
        statusLabel->show();
    }
    else
    {
        ipLabel->show();
        ipLineEdit->show();
        portLabel->show();
        portLineEdit->show();
        statusLabel->hide();
    }
}

void ShipSetupMenu::setStatusLabelText(QString text)
{
    statusLabel->setText(text);
}

void ShipSetupMenu::ipEdited()
{
    *ipAdress = ipLineEdit->text();
}

void ShipSetupMenu::portEdited()
{
    *port = portLineEdit->text().toInt();
}



