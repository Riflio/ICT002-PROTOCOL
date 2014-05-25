/*******************************
 * Реализация ICT Protocol (02)04 на Qt
 * Мне глубоко пофиг кто и что будет делать с этим классом.
 * Всё равно покачто он не доведён до конца.
 *
 * PavelK.ru
 ******************************/

#include "ictprotocol.h"


#include <QDebug>
#include <QApplication> //-- исользуется внизу в говнокоде

ICTProtocol::ICTProtocol(QObject *parent) :
    QObject(parent)
{
    serial=new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

bool ICTProtocol::autoInit()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
           init(info.portName());
           writeCode(0x0); //TODO: WHY????
           getStatus();
           if (getStatus()!=-1) {
                return true;
           } else {
               serial->close();
           }
    }
    emit exception(ErCodes::NOT_CONNECTED);
    return false;
}

ICTProtocol::~ICTProtocol()
{
    serial->close();
}

bool ICTProtocol::init(QString port) {
    portName=port;
    serial->setPortName(port);
    if (serial->open(QIODevice::ReadWrite)) {
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setParity(QSerialPort::EvenParity);
        serial->setDataBits(QSerialPort::Data8);
    } else {
        qDebug()<<"ICTProtocol: Serial port not open!";
        return false;
    }
    qDebug()<<"ICT Protocol: Serial port opened:"<<port;
    return true;

}

void ICTProtocol::readData()
{
    QByteArray data = serial->readAll();
    foreach (int code, data) {
        code=abs(code);
        qDebug()<<"ICTProtocol::readCode:"<<code;
        switch (code) {
            case 128:
            case 113:
                emit powerUp(code);
                break;
            case 38:
                emit inhibitStatus();
                break;
            case 127:
                emit escrow();
                break;
            case 64:
            case 65:
            case 66:
            case 67:
            case 68:
                emit responceBilValue(code-63);
                break;
            case 16:
                emit responceBillAccepted();
                break;
            case 41:
                emit responseBillRejected();
                break;

            default: //-- Ничего хорошего больше впринципе к нам прийти не может
                emit exception(code);
                break;
        }

    }
}

void ICTProtocol::writeCode(char code)
{
    if (!serial->isOpen()) {
        emit exception(ErCodes::NOT_CONNECTED);
        return;
    }
    QByteArray data;
    data.append(code);
    serial->write(data);
    serial->waitForBytesWritten(-1);
    serial->flush();
}


void ICTProtocol::reset() {
    writeCode(0x30);
}

void ICTProtocol::enable() {
    writeCode(0x3e);
}

void ICTProtocol::disable() {
    writeCode(0x5e);
}

void ICTProtocol::activate() {
    writeCode(0x02);
}

void ICTProtocol::responceBillAccept() {
    writeCode(0x02);
}

void ICTProtocol::responceBillReject() {
     writeCode(0x0f);
}


int ICTProtocol::getStatus() {
    //TODO: Серьёзно подумать, насколько реально нужен следующий говнокод и может перенести пора всё в другой поток...
    disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    writeCode(0x0c);
    int i=0;
    serial->waitForReadyRead(500);
    while(serial->bytesAvailable()==0) { //-- Ждём, пока не ответят
        QApplication::processEvents();
        i++;
        if (i>10000) return -1;
    }
    QByteArray data = serial->readAll();    
    int ercode = data[0];
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    return ercode;
}












