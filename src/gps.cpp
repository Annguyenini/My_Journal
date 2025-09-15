#include "gps.h"
#include "mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject> 


// void GPS::setUpGps()
// {
//     if (!_mainlayout) {
//         qWarning() << "Main layout is not initialized!";
//         return;
//     }
//      _cityLabel = new QLabel(this);
//     _cityLabel->setObjectName("City-Label");
//     _cityLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
//     _cityLabel->setText("test");

//     _coordinatesLabel = new QLabel(this);
//     _coordinatesLabel->setObjectName("Coordinates-Label");
//     _coordinatesLabel->setStyleSheet("color: white; font-size: 30px");
//     _mainlayout->addWidget(_cityLabel);
//     _mainlayout->addWidget( _coordinatesLabel);
//     qDebug()<<"Pass";
// }
GPS::GPS(QVBoxLayout* layout,QObject *parent) : QObject(parent),_mainlayout(layout),
_cityLabel (nullptr),_coordinatesLabel (nullptr),_workerThread(nullptr),_worker(nullptr)
{
}

// void GPS::setUpGps() {
//     _cityLabel =new QLabel("City-Label");
//     _cityLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
//     _cityLabel->setText("test");

//     _coordinatesLabel =new QLabel("Coordinates-Label");
//     _coordinatesLabel->setStyleSheet("color: white; font-size: 30px");

//     _mainlayout->addWidget(_cityLabel);
//     _mainlayout->addWidget(_coordinatesLabel);
void GPS::setUpGps(){
    _cityLabel = new QLabel();
    _cityLabel->setObjectName("City-Label");
    _cityLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
    _coordinatesLabel = new QLabel();
    _coordinatesLabel->setObjectName("Coordinates-Label");
    _coordinatesLabel->setStyleSheet("color: white; font-size: 30px");
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
// GPS::GPS(){
//     _cityLabel = new QLabel();
//     _cityLabel->setObjectName("City-Label");
//     _cityLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
//     _coordinatesLabel = new QLabel();
//     _coordinatesLabel->setObjectName("Coordinates-Label");
//     _coordinatesLabel->setStyleSheet("color: white; font-size: 30px");
//     _mainlayout->insertWidget(1, _cityLabel);
//     _mainlayout->insertWidget(2, _coordinatesLabel);
//     qDebug()<<"Pass";
//     _workerThread = new QThread(this);
//     _worker = new GPSWorker();
//     _worker->moveToThread(_workerThread);

//     connect(_workerThread, &QThread::started, _worker, &GPSWorker::initialPort);
//     connect(_workerThread, &QThread::started, _worker, &GPSWorker::startReadingFromGps);
//     connect(_worker, &GPSWorker::coordinatesUpdate, this, &GPS::handleCoordinatesUpdate, Qt::QueuedConnection);
//     connect(_worker, &GPSWorker::cityChanged, this, &GPS::handleCityUpdate, Qt::QueuedConnection);
//     connect(_workerThread, &QThread::finished, _worker, &QObject::deleteLater);
//     connect(_worker, &QObject::destroyed, _workerThread, &QThread::quit);
//     connect(_workerThread, &QThread::finished, _workerThread, &QObject::deleteLater);
//     qDebug()<<"before start thread";
//     qDebug() << "Current thread ID:" << QThread::currentThreadId();
//     _workerThread->start();
// }

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
GPS::~GPS(){
//      if (_workerThread) {
//         _workerThread->quit();
//         _workerThread->wait();
//     }
    delete _cityLabel;
    delete _coordinatesLabel;
}