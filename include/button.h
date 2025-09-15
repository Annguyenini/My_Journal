#ifndef BUTTONS_H
#define BUTTONS_H
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "configure.h"
class Buttons :public QObject{
    Q_OBJECT
    private:
        QVBoxLayout* _mainlayout;
        QHBoxLayout* _buttonsLayout;
        QPushButton* _tripBtn;
        QPushButton* _cameraBtn;
        QPushButton* _reloadBtn;
        QPushButton* _albumBtn;
        QPushButton* _snapPictureBtn;
        QPushButton* _recordBtn;
        QPushButton* _photoSettingBtn;
        
    public:
        Buttons(QVBoxLayout* mainlayout, QObject* parent);
        void setUpButtons();
        void cameraButtons();
        void showNormalButtons();
        Q_SIGNAL void callAlbum();
        Q_SIGNAL void callCamera();
        Q_SIGNAL void callTripMode();
        Q_SIGNAL void callReloadMap();
        Q_SIGNAL void callSnap();
        Q_SIGNAL void callRecord();
        Q_SIGNAL void callPhotoSetting();
};

#endif