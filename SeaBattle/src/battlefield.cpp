#include "battlefield.h"
#include <QMouseEvent>
#include <math.h>
#include <time.h>
#include <iostream>
#include <QtMultimedia/QSound>

// конструктор
BattleField::BattleField(GAME_STATUS *_gstatus, BattleStat *_batStats, QWidget *parent) :
    QWidget(parent)
{
    gstatus = _gstatus;
    batStats = _batStats;
    field_size = CELL * 10 + 2 * FIELD_OFFSET; // размер виджета поля = 10 клток + 2 отступа от края
    activeShip = NULL;
    conflictShip = NULL;
    shipMoving = false;
    setupMode = false;
    battleMode = false;
    clicked = false;

    for(int i = 0 ; i < 10 ; i++)
    {
        for(int j = 0 ; j < 10 ; j++)
        {
            field[i][j] = EMPTY;
        }
    }
    setFixedSize(field_size, field_size);
}

// обработчик события отрисовки виджета поля
void BattleField::paintEvent(QPaintEvent *event)
{
    QPainter painter(this); // рисовальщик
    QPen pen; // карандаш для отрисовки контуров объектов
    pen.setColor(Qt::darkCyan); // устанавливаем цвет карандаша
    pen.setWidth(3); // устанавливаем толшину карандаша
    painter.setPen(pen); // задаем для рисовальщика карандаш
    // рисуем рамку поля
    painter.drawRect(FIELD_OFFSET - 2, FIELD_OFFSET - 2, field_size - 2 * FIELD_OFFSET + 4, field_size - 2 * FIELD_OFFSET + 4);
    // изменяем цвет и толщину карандаша
    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    // отрисовываем поле
    painter.drawRect(FIELD_OFFSET, FIELD_OFFSET, field_size - 2 * FIELD_OFFSET, field_size - 2 * FIELD_OFFSET); // основной квадрат
    // решетка
    for(int i = FIELD_OFFSET + CELL ; i <= (field_size - FIELD_OFFSET - CELL) ; i += CELL)
        painter.drawLine(FIELD_OFFSET, i, field_size - FIELD_OFFSET, i); // вертикальные линии
    for(int i = FIELD_OFFSET + CELL ; i <= (field_size - FIELD_OFFSET - CELL) ; i += CELL)
        painter.drawLine(i, FIELD_OFFSET, i, field_size - FIELD_OFFSET); // горизонтальные линии
    QFont font; // шрифт
    font.setPixelSize(15); // задаем размер шрифта в пикселях
    painter.setFont(font); // передаем рисовальщику шрифт
    // изменяем цвет и толщину карандаша
    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    for(int i = 0 ; i < 10 ; i++)
        painter.drawText(0, FIELD_OFFSET + i * CELL + 18, QString::number(i + 1)); // отрисовываем цифры
    for(int i = 0 ; i < 10 ; i++)
        painter.drawText(FIELD_OFFSET + i * CELL + 8, FIELD_OFFSET - 5, QString(abc[i])); // отрисовываем буквы

    // сначала отрисовываем все корабли кроме активного, для того чтобы если при перетаскивании или конфликте
    // располжения конфликтный или активный корабль отрисовался поверх остальных
    for(int i = 0 ; i < ships.size() ; i++)
        if(ships.at(i) != activeShip)
            ships.at(i)->draw(&painter);

    // отрисовываем конфликтный или активный корабль
    if(conflictShip)
        conflictShip->draw(&painter);
    else
        if(activeShip)
            activeShip->draw(&painter);
    // отрисовываем статусы клеток поверх всего
    for(int i = 0 ; i < 10 ; i++)
    {
        for(int j = 0 ; j < 10 ; j++)
        {
            switch (field[i][j]) {
            case EMPTY:
                break;
            case KILLED:
            case DAMAGED:
                drawDamaged(&painter, j, i);
                break;
            case DOT:
                drawDot(&painter, j, i);
                break;
            }
        }
    }
}

// рисование точки в клетке (x,y)
void BattleField::drawDot(QPainter* painter, int x, int y)
{
    QPen pen; // карандаш для отрисовки контуров объектов
    QBrush brush; // кисть, определяет цвет заполнения фигур
    pen.setColor(Qt::black); // устанвливаем цвет карандаша
    painter->setPen(pen); // передаем карандаш рисовальщику
    brush.setStyle(Qt::SolidPattern); // определяем стиль закраски офигур (SolidPattern - сплошная закраска)
    brush.setColor(Qt::black); // устанвлваем цвет кисти
    painter->setBrush(brush); // передаем кисть рисовальщику
    painter->drawEllipse(QPoint(FIELD_OFFSET + x * CELL + CELL / 2, FIELD_OFFSET + y * CELL + CELL / 2), DOTR, DOTR); // рисуем точку
}

// рисование подбития
void BattleField::drawDamaged(QPainter* painter, int x, int y)
{
    QPen pen; // карандаш для отрисовки контуров объектов
    pen.setColor(Qt::red); // устанвливаем цвет карандаша
    pen.setWidth(2); // устанавливаем толшину карандаша
    painter->setPen(pen); // передаем карандаш рисовальщику
    // рисуем крестик из двух линий
    painter->drawLine(FIELD_OFFSET + x * CELL, FIELD_OFFSET + y * CELL, FIELD_OFFSET + (x + 1) * CELL, FIELD_OFFSET + (y + 1) * CELL);
    painter->drawLine(FIELD_OFFSET + x * CELL, FIELD_OFFSET + (y + 1) * CELL, FIELD_OFFSET + (x + 1) * CELL, FIELD_OFFSET + y  * CELL);
}

// обработчик нажатия мыши на поле
void BattleField::mousePressEvent(QMouseEvent *event)
{
    if(setupMode) // если поле в режиме редактирования расстановки кораблей
    {
        if(event->button() == Qt::LeftButton) // если нажата левая кнопка мыши
        {
            // на случай если мышка нажата на клетку в которой нет корбля делаем текущий активный корабль неактивным
            if(activeShip)
                activeShip->setActive(false);
            activeShip = NULL;
            // уведомляем меню расстановки о том что корабль деактивирован
            //(чтобы сделать неактивными кнопки удаления и поврота)
            emit shipActivated(false, -1);
            // проходим по списку кораблей и проверяем не принадлежит ли текущее положение курсора одному из кораблей
            for(int i = 0 ; i < ships.size() ; i++)
                if(ships.at(i)->onShip(event->pos())) // проверка
                {
                    // если определили что клинули на один из кораблей, активным,
                    // если есть конфликт расстановки, может стать только конфликтный корабль
                    if(conflictShip)
                    {
                        if(conflictShip == ships.at(i)) // кликнутный корабль - конфликтный
                        {
                            // делем корабль активным
                            activeShip = ships.at(i);
                            activeShip->setActive(true);
                            // уведомляем меню расстановки о том что корабль активирован
                            emit shipActivated(true, activeShip->getSize());
                            // устанавливаем флаг перетаскивания корабля
                            // пока этот флаг в значении true активный корабль будет перемещаться по полю согласно перемещению курсора
                            shipMoving = true;
                            // запоминаем текущую позицию курсора(для расчета его смещения)
                            lastPoint = event->pos();
                            // делаем перемещаемый корабль неконфликтным(чтобы при перемещении он подсвечивался зеленым)
                            if(conflictShip)
                                conflictShip->setConflict(false);
                            conflictShip = NULL;
                            break; // выход из цикла
                        }
                    }
                    else // если нет конфликта расстановки
                    {
                        // активируме корабль на который кликнули
                        activeShip = ships.at(i);
                        activeShip->setActive(true);
                        emit shipActivated(true, activeShip->getSize());
                        // устанавливаем флаг перетаскивания корабля
                        shipMoving = true;
                        // запоминаем текущую позицию курсора(для расчета его смещения)
                        lastPoint = event->pos();
                        break; // выход из цикла
                    }
                }
        }
        if(event->button() == Qt::RightButton) // если нажата правая кнопка мыши
        {
            // на случай если мышка нажата на клетку в которой нет корбля делаем текущий активный корабль неактивным
            if(activeShip)
                activeShip->setActive(false);
            activeShip = NULL;
            // уведомляем меню расстановки о том что корабль деактивирован
            //(чтобы сделать неактивными кнопки удаления и поврота)
            emit shipActivated(false, -1);
            // проходим по списку кораблей и проверяем не принадлежит ли текущее положение курсора одному из кораблей
            for(int i = 0 ; i < ships.size() ; i++)
                if(ships.at(i)->onShip(event->pos())) // проверка
                {
                    // если определили что клинули на один из кораблей, активным,
                    // если есть конфликт расстановки, может стать только конфликтный корабль
                    if(conflictShip)
                    {
                        if(conflictShip == ships.at(i)) // кликнутный корабль - конфликтный
                        {
                            // делем корабль активным
                            activeShip = ships.at(i);
                            activeShip->setActive(true);
                            // уведомляем меню расстановки о том что корабль активирован
                            emit shipActivated(true, activeShip->getSize());
                            activeShip->rotate(); // поворачиваем активный корабль
                            // проверяем наличие конфликта расстановки после поврота корабля
                            if(activeShip->isIntercepts())
                            {
                                // делаем кликнутый корабль конфликтным
                                conflictShip = activeShip;
                                conflictShip->setConflict(true);
                                // уведомляем меню расстановки о конфликте расстановки
                                emit shipConflict(true);
                            }
                            else
                            {
                                if(conflictShip)
                                    conflictShip->setConflict(false);
                                conflictShip = NULL;
                                // уведомляем меню расстановки об отсутствии конфликта
                                emit shipConflict(false);
                            }
                            break; // выход из цикла
                        }
                    }
                    else // если нет конфликта расстановки
                    {
                        // активируме корабль на который кликнули
                        activeShip = ships.at(i);
                        activeShip->setActive(true);
                        // уведомляем меню расстановки о том что корабль активирован
                        emit shipActivated(true, activeShip->getSize());
                        activeShip->rotate(); // поворачиваем активный корабль
                        // проверяем наличие конфликта расстановки после поврота корабля
                        if(activeShip->isIntercepts())
                        {
                            // делаем кликнутый корабль конфликтным
                            conflictShip = activeShip;
                            conflictShip->setConflict(true);
                            // уведомляем меню расстановки о конфликте расстановки
                            emit shipConflict(true);
                        }
                        else
                        {
                            if(conflictShip)
                                conflictShip->setConflict(false);
                            conflictShip = NULL;
                            // уведомляем меню расстановки об отсутствии конфликта
                            emit shipConflict(false);
                        }
                        break; // выход из цикла
                    }
                }
        }
    }
    // если поле в режиме боя и мой ход
    // в теле if переменая clicked устанавливается в true
    // чтобы до прихода ответа на ход нажатия мышки больше не обрабатывались
    // clicked будет снова установлена в значение false когда ход перейдет ко мне от противника
    // или если после ответа на текущий клик ход остается за мной
    if(clicked == false && battleMode && *gstatus == YOUR_TURN)
    {
        int x, y;
        // преобразуем коодинаты точки на виджете в номера клеток поля
        x = (event->pos().x() - FIELD_OFFSET) / CELL;
        y = (event->pos().y() - FIELD_OFFSET) / CELL;
        if(field[y][x] == EMPTY) // проверям что в данную клетку мы еще не ходили
        {
            clicked = true;
            emit updateStatusLabel(); // обновлейм статус хода в меню
            // высылаем сигнал о том что игрок походил в клтеку (x, y)
            // данный сигнал будет обработан слотом MainWindow::sendTurn(x, y),
            // который по протоколу TCP передаст кородинаты на приложение противника
            emit cellClicked(x, y);
        }
    }
    update(); // перерисовываем поле
}

// обработчик передвижения курсора по полю
void BattleField::mouseMoveEvent(QMouseEvent *event)
{
    if(setupMode) // перетаскивать корабли можно только в режиме расстановки
    {
        int deltax; // смещение курсора по горизонтали
        int deltay; // смещение курсора по вертикали
        // корабль перемешается толко если зажата кнопка мыши
        // shipMoving устанавливается в true при нажатии мыши и в false при ее отпускании
        if(shipMoving)
        {
            // если курсор по горизонтали за пределами поля корабль не должен перемещаться по этой оси
            if(event->pos().x() < FIELD_OFFSET || event->pos().x() > FIELD_OFFSET + 10 * CELL)
                deltax = 0;
            else
                // расчитываем смещение курсора после последнего вызова mousePressEvent или mouseMoveEvent
                deltax = event->pos().x() - lastPoint.x();
            // если курсор по вертикали за пределами поля корабль не должен перемещаться по этой оси
            if(event->pos().y() < FIELD_OFFSET || event->pos().y() > FIELD_OFFSET + 10 * CELL)
                deltay = 0;
            else
                // расчитываем смещение курсора после последнего вызова mousePressEvent или mouseMoveEvent
                deltay = event->pos().y() - lastPoint.y();
            activeShip->move(deltax, deltay); // перемещаем корабль
            lastPoint = event->pos(); // запоминаем текущее положение курсора для расчета следующего смещения
        }
        update(); // перерисовываем поле
    }
}

// обработчик отпускания кнопки мыши
void BattleField::mouseReleaseEvent(QMouseEvent *event)
{
    if(setupMode) // если режим расстановки
    {
        if(shipMoving) // если корабль был перемещен
        {
            int x, y;
            // при перемещении и отпускании кнопки мыши текущая позиция корабля может не соответствовать положению клеток поля
            // поэтому необходимо вычислить ближайщие клетки на которых может быть расположен корабль
            int up,bottom, rigth, left;
            left = FIELD_OFFSET + ((activeShip->getPosition().x() - FIELD_OFFSET) / CELL) * CELL;
            rigth = FIELD_OFFSET + ((activeShip->getPosition().x() - FIELD_OFFSET) / CELL) * CELL + CELL;
            up = FIELD_OFFSET + ((activeShip->getPosition().y() - FIELD_OFFSET) / CELL) * CELL;
            bottom = FIELD_OFFSET + ((activeShip->getPosition().y() - FIELD_OFFSET) / CELL) * CELL + CELL;
            if( activeShip->getPosition().x() - left <= CELL / 2) // ближе ли корабль к левой или правой клетке
                x = left;
            else
                x = rigth;
            if(activeShip->getPosition().y() - up <= CELL / 2) // ближе ли корабль к верхней или нижней клетке
                y = up;
            else
                y = bottom;
            activeShip->moveTo(x, y); // перемещаем корабль в найденое положение
            // проверяем наличие конфликта расстановки после перемещения корабля
            if(activeShip->isIntercepts())
            {
                // делаем кликнутый корабль конфликтным
                conflictShip = activeShip;
                conflictShip->setConflict(true);
                // уведомляем меню расстановки о конфликте расстановки
                emit shipConflict(true);
            }
            else
            {
                if(conflictShip)
                    conflictShip->setConflict(false);
                conflictShip = NULL;
                // уведомляем меню расстановки об отсутствии конфликта
                emit shipConflict(false);
            }
        }
        shipMoving = false; // перемещение корабля закончено
        update(); // перерисовываем поле
    }
}

// поворот корабля
void BattleField::rotateActiveShip()
{
    // если есть активный корабль, то поворачиваем его
    if(activeShip)
        activeShip->rotate();
    // проверяем наличие конфликта расстановки после поворота корабля
    if(activeShip->isIntercepts())
    {
        // делаем кликнутый корабль конфликтным
        conflictShip = activeShip;
        conflictShip->setConflict(true);
        // уведомляем меню расстановки о конфликте расстановки
        emit shipConflict(true);
    }
    else
    {
        if(conflictShip)
            conflictShip->setConflict(false);
        conflictShip = NULL;
        // уведомляем меню расстановки об отсутствии конфликта
        emit shipConflict(false);
    }
    update();
}

// добавление на поле корабля размера size
void BattleField::addShipToField(int size)
{
    // новый корабль устанавливается вертикально ориентированным начиная с левого верзнего угла поля
    Ship* cur = new Ship(QPoint(FIELD_OFFSET, FIELD_OFFSET), size, VERTICAL, &ships);
    ships.append(cur); // добавляем новый корабль в список кораблей
    // проверяем наличие конфликта расстановки
    if(cur->isIntercepts())
    {
        // делаем добавленный корабль конфликтным
        conflictShip = cur;
        conflictShip->setConflict(true);
        // уведомляем меню расстановки о конфликте расстановки
        emit shipConflict(true);
    }
    else
    {
        if(conflictShip)
            conflictShip->setConflict(false);
        conflictShip = NULL;
        // уведомляем меню расстановки об отсутствии конфликта
        emit shipConflict(false);
    }
    // деактивируем текущий активный корабль
    if(activeShip)
        activeShip->setActive(false);
    // делаем активным добавленный корабль
    activeShip = cur;
    activeShip->setActive(true);
    emit shipActivated(true, activeShip->getSize());
    update(); // перерисовываем поле
}

// удаление корабля с поля
void BattleField::deleteShipFromField()
{
    // проходим по списку кораблей и находим активный корабль
    for(int i = 0 ; i < ships.size() ; i++)
        if(ships.at(i) == activeShip) // проверка
        {
            ships.removeAt(i); // удаляем корабль из списка
            if(activeShip == conflictShip) // если удалем конфликтный корабль то сообщаем меню расстановки о разрешении конфликта
            {
                conflictShip = NULL;
                emit shipConflict(false);
            }
            delete activeShip; // удаляем объект
            activeShip = NULL;
            emit shipActivated(false, -1);
            break;
        }
    update();
}

// сброс расстановки
void BattleField::reset()
{
    activeShip = NULL;
    conflictShip = NULL;
    setupMode = false;
    shipMoving = false;
    // очищаем список кораблей
    while(ships.size() != 0)
    {
        Ship* cur = ships.first();
        ships.pop_front();
        delete cur;
    }
}

// генерация случайной расстановки кораблей
void BattleField::generateRandom()
{
    reset(); // сбрасываем расстановку
    setupMode = true;
    srand(time(NULL)); // инициализируем генератор псевдослучайных чисел
    direction dir; // ориентация корабля
    Ship* cur; // указатель на корабль который пытаемся поставить на поле
    // модули для координат верхнего левого угла корабля по горизонтали и верикали
    int xmod, ymod;
    int x, y;

    // ставим однопалубные корабли
    for( int i = 0 ; i < SHIP1_AMOUNT ; i++)
    {
        // случайно выбираем расположение для корабля до тех пор, пока его положение не будет
        // конфликтовать с уже установленными на поле кораблями
        do
        {
            // выбираем ориентациию
            if(rand() % 2 == 0)
                dir = VERTICAL;
            else
                dir = HORIZONTAL;
            // выбираем клетку для левого верхнего квадрата корабля
            switch(dir)
            {
            case VERTICAL:
                xmod = 10;
                ymod = 10 - SHIP1 + 1;
                break;
            case HORIZONTAL:
                ymod = 10;
                xmod = 10 - SHIP1 + 1;
                break;
            }
            // пересчитываем номер клетки в кординаты на виджете
            x = (rand() % xmod) * CELL;
            y = (rand() % ymod) * CELL;
            cur = new Ship(QPoint(FIELD_OFFSET + x, FIELD_OFFSET + y), SHIP1, dir, &ships);
        }
        while(cur->isIntercepts());// проверка наложения кораблей
        ships.append(cur); // добавление корабля в список кораблей на поле
    }

    // ставим двухпалубные корабли
    for( int i = 0 ; i < SHIP2_AMOUNT ; i++)
    {
        // случайно выбираем расположение для корабля до тех пор, пока его положение не будет
        // конфликтовать с уже установленными на поле кораблями
        do
        {
            // выбираем ориентациию
            if(rand() % 2 == 0)
                dir = VERTICAL;
            else
                dir = HORIZONTAL;
            // выбираем клетку для левого верхнего квадрата корабля
            switch(dir)
            {
            case VERTICAL:
                xmod = 10;
                ymod = 10 - SHIP2 + 1;
                break;
            case HORIZONTAL:
                ymod = 10;
                xmod = 10 - SHIP2 + 1;
                break;
            }
            // пересчитываем номер клетки в кординаты на виджете
            x = (rand() % xmod) * CELL;
            y = (rand() % ymod) * CELL;
            cur = new Ship(QPoint(FIELD_OFFSET + x, FIELD_OFFSET + y), SHIP2, dir, &ships);
        }
        while(cur->isIntercepts());// проверка наложения кораблей
        ships.append(cur);// добавление корабля в список кораблей на поле
    }

    // ставим трехпалубные корабли
    for( int i = 0 ; i < SHIP3_AMOUNT ; i++)
    {
        // случайно выбираем расположение для корабля до тех пор, пока его положение не будет
        // конфликтовать с уже установленными на поле кораблями
        do
        {
            // выбираем ориентациию
            if(rand() % 2 == 0)
                dir = VERTICAL;
            else
                dir = HORIZONTAL;
            // выбираем клетку для левого верхнего квадрата корабля
            switch(dir)
            {
            case VERTICAL:
                xmod = 10;
                ymod = 10 - SHIP3 + 1;
                break;
            case HORIZONTAL:
                ymod = 10;
                xmod = 10 - SHIP3 + 1;
                break;
            }
            // пересчитываем номер клетки в кординаты на виджете
            x = (rand() % xmod) * CELL;
            y = (rand() % ymod) * CELL;
            cur = new Ship(QPoint(FIELD_OFFSET + x, FIELD_OFFSET + y), SHIP3, dir, &ships);
        }
        while(cur->isIntercepts()); // проверка наложения кораблей
        ships.append(cur); // добавление корабля в список кораблей на поле
    }

    // ставим четырехпалубные корабли
    for( int i = 0 ; i < SHIP4_AMOUNT ; i++)
    {
        // случайно выбираем расположение для корабля до тех пор, пока его положение не будет
        // конфликтовать с уже установленными на поле кораблями
        do
        {
            // выбираем ориентациию
            if(rand() % 2 == 0)
                dir = VERTICAL;
            else
                dir = HORIZONTAL;
            // выбираем клетку для левого верхнего квадрата корабля
            switch(dir)
            {
            case VERTICAL:
                xmod = 10;
                ymod = 10 - SHIP4 + 1;
                break;
            case HORIZONTAL:
                ymod = 10;
                xmod = 10 - SHIP4 + 1;
                break;
            }
            // пересчитываем номер клетки в кординаты на виджете
            x = (rand() % xmod) * CELL;
            y = (rand() % ymod) * CELL;
            cur = new Ship(QPoint(FIELD_OFFSET + x, FIELD_OFFSET + y), SHIP4, dir, &ships);
        }
        while(cur->isIntercepts()); // проверка наложения кораблей
        ships.append(cur); // добавление корабля в список кораблей на поле
    }
    update(); // перерисовываем поле
}

// установка значения режима расстановки кораблей
void BattleField::setSetupMode(bool mode)
{
    setupMode = mode;
}

// установка значения игрового режима
void BattleField::setBattleMode(bool mode)
{
    battleMode = mode;
}

// проверка хода противника
void BattleField::checkTurn(int x, int y)
{
    CELL_STATUS st = DOT; // считаем что изначально в клетке пусто
    Ship* killed = NULL;
    // проходим по списку кораблей проверяя не находится ли в клетке в которую походил проотивник наш корабль
    for(int i = 0; i < ships.size() ; i++)
    {
        Ship* cur = ships.at(i);
        if(cur->onShip(QPoint(FIELD_OFFSET + x * CELL + CELL / 2, FIELD_OFFSET + y * CELL + CELL / 2))) // проверка
        {
            // если в клетке есть корабль - наносим ему поврждение
            // если damageShip вернет true то корабль убит
            if(cur->damageShip())
            {
                killed = cur;
                st = KILLED;
                batStats->shipKilled(ME, killed->getSize()); // изменяем игровую статистику
                fillShipDots(killed->getPosition(), killed->getSize(), killed->getDirection()); // окружаем корабль точками
            }
            else // корабль подбит
            {
                st = DAMAGED;
            }
            break; // выход из цикла
        }
    }
    field[y][x] = st; // устанавливаем статус для клетки в которую походил протиник
    if(st == DOT) // если промах то мой ход
        *gstatus = YOUR_TURN;
    // посылаем ответ на ход противнику
    // данный сигнал будет обработан слотом MainWindow::sendTurnResponse(x, y),
    // который по протоколу TCP передаст результат хода на приложение противника
    emit turnResponse(x, y, st, killed);
    emit updateStatusLabel(); // обнавляем статус хода и отображаемую статистику
    batStats->checkBattleStatus(); // провераем не победил ли один из игроков
    if(st == KILLED || st == DAMAGED)
        QSound::play(":/sounds/Sounds/boom.wav"); // звук подбития
    else
        QSound::play(":/sounds/Sounds/splash-01.wav"); // звук всплеска
    update(); // перерисовываем виджет
}

// проверка ответа противника на ход
// если противник ответил что его корабль убит, то pos, size, dir
// содержат данные для отрисовки убитого корабля
void BattleField::checkTurnResponse(int x, int y, CELL_STATUS st, QPoint pos, int size, direction dir)
{
    field[y][x] = st; // устанавливаем статус клетки поля для которой получен ответ
    clicked = false;
    if(st == KILLED) // корабль противника убит
    {
        // добавляем на поле убитый корабль
        Ship* cur = new Ship(QPoint(pos.x(), pos.y()), size, dir, &ships);
        ships.append(cur);
        fillShipDots(cur->getPosition(), cur->getSize(), cur->getDirection()); // окружаем корабль точками
        batStats->shipKilled(ENEMY, size); // обновляем статистику игры
    }
    if(st == DOT) // если промах то ход переходит к противнику
        *gstatus = ENEMY_TURN;
    emit updateStatusLabel(); // обнавляем статус хода и отображаемую статистику
    batStats->checkBattleStatus(); // провераем не победил ли один из игроков
    if(st == KILLED || st == DAMAGED)
        QSound::play(":/sounds/Sounds/boom.wav"); // звук подбития
    else
        QSound::play(":/sounds/Sounds/splash-01.wav"); // звук всплеска
    update(); // перерисовываем виджет
}

// установка точек вокруг убитого корабля
void BattleField::fillShipDots(QPoint pos, int size, direction dir)
{
    int x1, x2, y1, y2;
    // вычисляем координаты клеток, окружающие корабль
    int x = (pos.x() - FIELD_OFFSET) / CELL;
    int y = (pos.y() - FIELD_OFFSET) / CELL;
    switch(dir)
    {
        case HORIZONTAL:
        {
            x1 = x - 1 >= 0 ? x - 1 : 0;
            y1 = y - 1 >= 0 ? y - 1 : 0;
            x2 = x + size <= 9 ? x + size : 9;
            y2 = y + 1  <= 10 ? y + 1 : 9;
            break;
        }
        case VERTICAL:
        {
            x1 = x - 1 >= 0 ? x - 1 : 0;
            y1 = y - 1 >= 0 ? y - 1 : 0;
            x2 = x + 1  <= 9 ? x + 1 : 9;
            y2 = y + size <= 9? y + size : 9;
            break;
        }
    }

    // устанавливаем статусы окружающих клеток
    for(int i = x1 ; i <= x2 ; i++)
        if(field[y1][i] == EMPTY)
            field[y1][i] = DOT;
    for(int i = x1 ; i <= x2 ; i++)
        if(field[y2][i] == EMPTY)
            field[y2][i] = DOT;
    for(int i = y1 ; i <= y2 ; i++)
        if(field[i][x1] == EMPTY)
            field[i][x1] = DOT;
    for(int i = y1 ; i <= y2 ; i++)
        if(field[i][x2] == EMPTY)
            field[i][x2] = DOT;
}

// деактивация выделеного корабля
void BattleField::setNoActive()
{
    if(activeShip)
        activeShip->setActive(false);
    activeShip = NULL;
    if(conflictShip)
        conflictShip->setConflict(false);
    conflictShip = NULL;
}
