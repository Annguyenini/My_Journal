#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "button.h"
Buttons::Buttons(QVBoxLayout* mainlayout, QObject* parent) :_mainlayout(mainlayout), QObject(parent)
,_buttonsLayout(nullptr),_logBtn(nullptr),_cameraBtn(nullptr),_reloadBtn(nullptr),_albumBtn(nullptr)
{
}


void Buttons::setUpButtons(){
    _buttonsLayout =new QHBoxLayout();
    _albumBtn =new QPushButton("Album");
    _logBtn =new QPushButton("Log");
    _cameraBtn =new QPushButton("Camera");
    _reloadBtn =new QPushButton("Reload");
    _snapPictureBtn =new QPushButton("Snap");
    _recordBtn =mew QPushButton("Record");
    _albumBtn->setObjectName("Album-Button");

    _logBtn->setObjectName("Log-Button");
    _cameraBtn->setObjectName("Camera-Button");
    _reloadBtn->setObjectName("Reload-Button");
    for (QPushButton* btn :{_albumBtn, _logBtn, _cameraBtn, _reloadBtn}){
        btn->setStyleSheet(_btn_properties);
        _buttonsLayout->addWidget(btn);
        
    }
    connect(_albumBtn, &QPushButton::clicked, this, [this]{
        Q_EMIT callAlbum();
    });
    connect (_cameraBtn,&QPushButton::clicked, this, [this](){
        Q_EMIT callCamera();
    });
    connect (_logBtn,&QPushButton::clicked,this, [this](){
        Q_EMIT callLog();

    });
    connect (_reloadBtn,&QPushButton::clicked,this, [this](){
        Q_EMIT callReloadMap();
        
    });
    _mainlayout->addLayout(_buttonsLayout);
}

void Button::cameraButtons(){
    _logBtn->hide();
    _reloadBtn->hide();
    int i =1;
    for (QPushButton* btn:{_snapPictureBtn,_recordBtn}){
        btn ->setStyleSheet(_btn_properties);
        _buttonsLayout->insertWidget(i,btn);
        i++;
    }
    connect(_snapPictureBtn,&QPushButton::clicked, this, [this](){
        Q_EMIT callSnap();
    });
    
    connect(_recordBtn,&QPushButton::clicked, this, [this](){
        Q_EMIT callRecord();
    });
}
