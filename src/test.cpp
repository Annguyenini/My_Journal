#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSerialPort serial;
    MainWindow w;
    w.show();
    
    // Initialize the GPSWorker
    serial.setPortName("/dev/serial0");
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    if (!serial.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open serial port!";
        return -1;
    }
    QObject::connect(&serial, &QSerialPort::readyRead, [&]() {
        QByteArray data = serial.readAll();
        QString line = QString::fromUtf8(data);
        QStringList parts = line.split(',');
        if (parts[0] == "$GPRMC") {
            float lat = parts[3].toFloat();
            float lng = parts[5].toFloat();
            qDebug() << "Latitude:" << lat << "Longitude:" << lng;
        } else if (parts[0] == "$GPVTG") {
            float speed = parts[7].toFloat();
            qDebug() << "Speed:" << speed;
        } else {
            qDebug() << "Fail to read data or there is no good data!";
        }
    });

    return a.exec();
}
