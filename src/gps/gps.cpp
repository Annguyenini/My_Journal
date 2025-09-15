#include "gps.h"
#include "mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject> 



GPS::GPS(QVBoxLayout* layout,QObject *parent) : QObject(parent),_mainlayout(layout),
_cityLabel (nullptr),_coordinatesLabel (nullptr),_workerThread(nullptr),_worker(nullptr)
{
}

void GPS::setUpGps(){
    _cityLabel = new QLabel();
    _cityLabel->setObjectName("City-Label");
    _cityLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
    _cityLabel->setAlignment(Qt::AlignCenter);
    _coordinatesLabel = new QLabel();
    _coordinatesLabel->setObjectName("Coordinates-Label");
    _coordinatesLabel->setStyleSheet("color: white; font-size: 30px");
    _coordinatesLabel->setAlignment(Qt::AlignCenter);
    _mainlayout->insertWidget(1, _cityLabel);
    _mainlayout->insertWidget(2, _coordinatesLabel);
    qDebug()<<"Pass";
    _workerThread = new QThread(this);
    _worker = new GPSWorker();
    _worker->moveToThread(_workerThread);

    connect(_workerThread, &QThread::started, _worker, &GPSWorker::initialPort);
    connect(_workerThread, &QThread::started, _worker, &GPSWorker::startReadingFromGps);
    connect(_worker, &GPSWorker::coordinatesUpdate, this, &GPS::handleCoordinatesUpdate, Qt::QueuedConnection);
    connect(_worker, &GPSWorker::cityChanged, this, &GPS::handleCityUpdate, Qt::QueuedConnection);
    connect(_workerThread, &QThread::finished, _worker, &QObject::deleteLater);
    connect(_worker, &QObject::destroyed, _workerThread, &QThread::quit);
    connect(_workerThread, &QThread::finished, _workerThread, &QObject::deleteLater);
    qDebug()<<"before start thread";
    qDebug() << "Current thread ID:" << QThread::currentThreadId();
    _workerThread->start();
}

void GPS::handleCityUpdate(const std::string &nameId, const std::string &realName) {
    _cityLabel->setText(QString::fromStdString(realName));
}
void GPS::handleCoordinatesUpdate(float lat, float lng) {
    _coordinatesLabel->setText(QString("Lat: %1, Lng: %2").arg(lat).arg(lng));
}
// void GPS::stopWorker() {
//     Q_EMIT stopWorker();
// }
void GPS::stop() {
    if (_workerThread) {
        
        _workerThread->quit();
        _workerThread->wait();
    }
}
void GPS::hideLabel(){
    _cityLabel->hide();
    _coordinatesLabel->hide();
    _mainlayout->removeWidget(_cityLabel);
    _mainlayout->removeWidget(_coordinatesLabel);
    return;
}
void GPS::showLabel(){
    _mainlayout->insertWidget(1, _cityLabel);
    _mainlayout->insertWidget(2, _coordinatesLabel);
    _cityLabel->show();
    _coordinatesLabel->show();
}
GPS::~GPS(){
//      if (_workerThread) {
//         _workerThread->quit();
//         _workerThread->wait();
//     }
    delete _cityLabel;
    delete _coordinatesLabel;
}