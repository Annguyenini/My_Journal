#include "mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject> 
 #include <QTimer>      // <-- here it is
#include <QDateTime>


StatusBar::StatusBar(QVBoxLayout* mainlayout, QObject* parent): QObject(parent),_mainlayout(mainlayout)
,_statusbar(nullptr),_batteryPer(nullptr),_optionBtn(nullptr),_clockLabel(nullptr),_exitBtn(nullptr),_optionMenu(nullptr){};


void StatusBar::option_menu(){
    _optionMenu = new QMenu();
    _optionMenu->setStyleSheet(Config::instance().actionPro());
    _exitBtn =new QAction("Exit",this);
    _mapBtn =new QAction("Map", this);
    connect(_exitBtn, &QAction::triggered, this, [this](){
        QWidget * w = qobject_cast<QWidget*>(parent());
        if (w){
            w->close();
        }
    });
    connect (_mapBtn,&QAction::triggered,this,[this](){
        Q_EMIT changeBackground();
    });
    _optionMenu-> addAction(_exitBtn);
    _optionMenu ->addAction(_mapBtn);
}
void StatusBar::SetUpStatusBar(){
    _statusbar = new QHBoxLayout;
    _clockLabel= new QLabel();
    _clockLabel->setObjectName ("Status-Bar");
    _clockLabel->setStyleSheet("color: white; font-size: 15px; margin: 0; padding: 0;");

    _optionBtn = new QPushButton ("≡");
    _optionBtn->setStyleSheet(Config::instance().btnProperties());
    connect (_optionBtn, & QPushButton::clicked, this, [this](){
        this->option_menu();
        if(_optionMenu){
            _optionMenu->exec(_optionBtn->mapToGlobal(QPoint(0,_optionBtn->height())));
        }
    });

    _batteryPer = new QLabel("batterry");
    _batteryPer->setObjectName("Battery-percentage");
    _batteryPer->setStyleSheet("color: white; font-size: 15px;");
    
    _statusbar->addWidget(_optionBtn);
    _statusbar->addStretch(1);
    _statusbar->addWidget(_clockLabel);
    _statusbar->addStretch(1);
    _statusbar->addWidget(_batteryPer);
    _mainlayout->insertLayout(0,_statusbar);
     QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &StatusBar::updateTime);
    timer->start(1000);  // update every 1 second
    
}
void StatusBar:: updateTime() {
        QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
        _clockLabel->setText(currentTime);
    }
// void StatusBar::closeApplication(){
//     if (QWidget* widgetParent = qobject_cast<QWidget*>(parent())) {
//         widgetParent->close(); // this will close your main window safely
//     }
// }
StatusBar::~StatusBar(){
    delete _statusbar;
}