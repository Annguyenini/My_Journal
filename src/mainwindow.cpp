#include "mainwindow.h"
#include "gps.h"
#include "button.h"
#include "./ui_mainwindow.h"
#include <filesystem>
#include <any>
#include <thread>
#include <chrono>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>
#include <QTimer>  
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Class-Based Window");
    resize(800, 480);
    // showFullScreen();
    _central_widget = new QWidget(this);
    _central_widget->setStyleSheet("background-color: #292828;");
    setCentralWidget(_central_widget);
    _mainlayout = new QVBoxLayout (_central_widget);
    _mainlayout->setContentsMargins(0,0,0,0);
    _mainlayout->setSpacing(0);
    _central_widget->setLayout(_mainlayout);
    _stus = new StatusBar(_mainlayout,this);
    _stus -> SetUpStatusBar(); 
    _gps =new GPS(_mainlayout,this );
    _gps -> setUpGps();
    _map = new MAP(_mainlayout,this);
    _map->setUpMap();
    _map -> displayMap();
    _buttons = new Buttons(_mainlayout,this);
    _buttons ->setUpButtons();
    _album = new Album(_mainlayout,this);
    _currentActFeaPtr =_map;
    _trip = new Trip(_mainlayout);
    _trip ->setUpTrip();


    MainWindow::setUpConnectionWithButtons();
}
void MainWindow::setUpConnectionWithButtons(){
    connect(_buttons, &Buttons::callCamera, this, [this] { ////If no active feature at all
        std::string currentDir = std::filesystem::current_path().parent_path().string();
        std::string command = currentDir + "/src/camera-app/build/trip";
        int rc = std::system(command.c_str());
        if(rc!=0){
            qDebug() << "Failed to start camera app";
        } else {
            qDebug() << "Camera app started successfully";
        
        }

    qDebug() << "Fail at connect with camera!";
    });

    connect (_buttons,&Buttons::callReloadMap,this,[this](){
        _map->reloadMap();
    });
    
    connect(_buttons,&Buttons::callAlbum, this,[this](){
        bool isAlbumDisplay =_album ->displayAlbum();
        if(isAlbumDisplay==true){
            
            _gps->hideLabel();  
            _currentActFeaPtr = _album; 
            qDebug()<<"album open";
        }     
        else if(isAlbumDisplay==false){
            _gps->showLabel();
            _currentActFeaPtr.reset();
            _currentActFeaPtr = _map;
        }
        _map -> displayMap();
        qDebug()<<"Finished set up album";
        });
    connect (_stus,&StatusBar::changeBackground, this,[this](){
        qDebug()<<"called";
        _map->switchBackground();
    });
    connect(_buttons,&Buttons::callTripMode,this,[this](){
        qDebug()<<"called display trip";
        _trip ->displayTrip();
    });
}


// void MainWindow::timer(){
// }

MainWindow::~MainWindow()
{
    delete ui;
}
