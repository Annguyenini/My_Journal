#include <QWebEngineView>
#include <QUrl>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include "mainwindow.h"

MAP::MAP(QVBoxLayout* mainlayout, QObject* parent)
    : QObject(parent),
      _mainlayout(mainlayout),
      _maplabel(nullptr),
      _mainStack(nullptr),
      _mainBackgroundLabel(nullptr),
      _isMap(false),
      _isDisplay(false) {}

void MAP::setUpMap() {
    // Parent container widget
    _mainWidget = new QWidget();
    _mainStack = new QStackedLayout(_mainWidget);

    // Map widget
    _maplabel = new QWebEngineView(_mainWidget);
    _maplabel->setObjectName("Map-Label");
    _maplabel->load(QUrl("http://192.168.0.52:5000"));
    _maplabel->setMinimumSize(100, 100);
    _maplabel->hide();  // start hidden, background first

    // Background image
    _mainBackgroundLabel = new QLabel(_mainWidget);
    QPixmap pix(QString::fromStdString(Config::instance().mainBackgroundPath().string()));
    _mainBackgroundLabel->setPixmap(pix.scaled(800, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    _mainBackgroundLabel->setFixedSize(800, 100);
    _mainBackgroundLabel->setAlignment(Qt::AlignCenter);

    qDebug() << QString::fromStdString(Config::instance().mainBackgroundPath().string());

    // Add background first
    _mainStack->addWidget(_mainBackgroundLabel);
    _mainStack->addWidget(_maplabel);

    // Insert container widget into outer layout
    _mainlayout->insertWidget(3, _mainWidget);
}

void MAP::reloadMap() {
    if (_maplabel) {
        _maplabel->reload();
    }
}

void MAP::displayMap() {
    if (!_isDisplay) {
        _mainWidget->show();
        _isDisplay = true;
    } else {
        _mainWidget->hide();
        _isDisplay = false;
    }
}

void MAP::switchBackground() {
    if (!_isMap) {
        _mainStack->setCurrentWidget(_maplabel);
        _isMap = true;
    } else {
        _mainStack->setCurrentWidget(_mainBackgroundLabel);

        _isMap = false;
    }
}

MAP::~MAP() {
    // Children deleted automatically by parent (_mainWidget)
    delete _mainWidget;
}
