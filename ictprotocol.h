/*******************************
 * Реализация ICT Protocol (02)04 на Qt
 * Мне глубоко пофиг кто и что будет делать с этим классом.
 * Всё равно покачто он не доведён до конца.
 *
 * PavelK.ru
 ******************************/

#ifndef ICTPROTOCOL_H
#define ICTPROTOCOL_H

#include <QObject>
#include <QByteArray>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "math.h"

namespace ErCodes {
enum ErCodes {
    ResponseMotorFailure=32, //-- Серьёзная проблема с приводом
    ResponseCheckSumError, //-- не знаю
    ResponseBillJam, //-- Замятие купюры
    ResponseBillRemove, //-- Требуется обслуживание
    ResponseStackerOpen, //-- Открыт стеккер купюр
    ResponseSensorProblem, //-- Серьёзные проблемы с датчиками
    ResponseBillFish=39, //-- не знаю
    ResponseStackerProblem, //-- Серьёзная проблема со стекером
    ResponseBillReject,  //-- Купюрник выплюнул купюру (Не подлинная или не распознал)
    ResponseInvalidCommand, //-- Ошибка с командой
    ErrorExclusion=47, //-- Проблема ушла
    BillAcceptInhibit=94, //-- В спячке
    OK = 62,
    NOT_CONNECTED=-1
};
}


class ICTProtocol : public QObject
{
    Q_OBJECT
public:
    explicit ICTProtocol(QObject *parent = 0);
    ~ICTProtocol();

    bool autoInit();
    bool init(QString port);

private:
    QSerialPort * serial;
    QString portName;

public slots:
    void reset();
    void enable();
    void disable();
    void activate();

    void responceBillAccept(); //-- Принимаем валюту
    void responceBillReject(); //-- Не принимаем
    int getStatus(); //-- Отдаём статуc купюрника или ошибку

private slots:
    void readData();
    void writeCode(char);

signals:
    void powerUp(int);  //-- Подрубили питание или запросили коннект
    void inhibitStatus(); //-- Не дождался команды на активацию, ушёл в спяку
    void escrow(); //-- Валидатор проверяет валюту
    void responceBilValue(int); //-- Валюта проверена, отдаём её тип (первый, второй, третий (10, 50, 100 руб и тд) и тд в зависимости от прошивки)
    void responceBillAccepted(); //-- Приёмка и укладывание прошли успешно
    void responceBillFailure(); //-- Облом, можно ждать ошибки
    void exception(int); //-- Купюрник прислал ошибку
    void responseBillRejected(); //-- Купюрник выплюнул купюру
};

#endif // ICTPROTOCOL_H
