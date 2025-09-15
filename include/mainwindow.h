#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>   
#include <QLayout>  
#include <QString> 
#include <QWebEngineView>
#include<QStackedLayout>
#include <QUrl>
#include <string>
#include <any>
#include "button.h"
#include "gps.h"
#include "album.h"
#include "configure.h"
#include "trip.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MAP: public QObject{
    Q_OBJECT
    private:
        QVBoxLayout* _mainlayout;
        QWebEngineView* _maplabel;
        QWidget * _mainWidget;
        QStackedLayout* _mainStack;
        QLabel* _mainBackgroundLabel;
        bool _isDisplay = false;
        bool _isMap =false;
        
    public:
        MAP(QVBoxLayout* mainlayout,QObject* parent);
        ~MAP();
        void setUpMap();
        void reloadMap();
        void displayMap();
        void switchBackground();
};
class StatusBar: public QObject{
    Q_OBJECT
    private:
        QVBoxLayout* _mainlayout;
        QHBoxLayout* _statusbar;
        QLabel* _batteryPer;
        QPushButton* _optionBtn;
        QMenu* _optionMenu;
        QLabel* _clockLabel;
        QAction* _exitBtn;
        QAction* _mapBtn;

    public:
        explicit StatusBar(QVBoxLayout* mainlayoutn, QObject* parent =nullptr);
        ~StatusBar();
        void option_menu();
        void SetUpStatusBar();
        Q_SIGNAL void changeBackground();
        void updateTime();


    }; 

// class Feature : public QObject{
//     Q_OBJECT
//     public:
//         explicit Feature(QObject* parent);
//         virtual void startFeature
// }

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:  
    
    QVBoxLayout* _mainlayout;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUpConnectionWithButtons();

private:
    Ui::MainWindow *ui;
    QWidget* _central_widget;
    Buttons* _buttons;
    Album* _album;
    GPS* _gps;
    MAP* _map;
    StatusBar* _stus;
    Trip* _trip;
    QHBoxLayout _recordTimers;
    std::any _currentActFeaPtr;
    
    // auto* _currentActFeaPtr ;
};


#endif // MAINWINDOW_H
