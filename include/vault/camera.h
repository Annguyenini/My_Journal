#pragma once
#include <QObject>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include "libcam2opencv.h"
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include <QDialog>
#include<memory>
#include "gps.h"
#include "album.h"
#include "cameraworker.h"
#include "configure.h"

class Camera:public QObject {
    Q_OBJECT
public:
    explicit  Camera(QVBoxLayout* mainlayout);
    ~Camera();
    void setUpCamera();
    void StartCamera();
    void showCamera();
    void stopCamera();
    bool takePicture();
    void onSnapButtonClicked();
    void startRpiCamHello(const int& duration = 0);
    void timerForVideo();
    void showTimer();
    void hideTimer();
    Q_SIGNAL void recordFinished();
private:
    AlbumWorker* _albumObject;
    QLabel* _CameraLabel;
    QVBoxLayout* _mainlayout;
    CameraWorker* _camera;
    QHBoxLayout* _recordTimers;
    QPushButton* fiveTimer;
    QPushButton* tenTimer;
    QPushButton* fifteenTimer;
    QPushButton* twentyTimer;

    GPSWorker* _gpsObject;


    bool isCameraSettingDisplay = false;
    
    bool _isStarted =false;

    QSettings* _settings;
    std::filesystem::path _captureDB= "";
};
