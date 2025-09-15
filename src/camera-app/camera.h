#pragma once

#include <QObject>
#include <QMainWindow>

#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include <QDialog>
#include<memory>
#include <QSettings>
#include <QMutex>
#include <QMutexLocker>
#include <sqlite3.h>
#include <filesystem>
#include "cameraworker.h"


class Camera: public QMainWindow {
    Q_OBJECT
public:
    explicit Camera(QWidget *parent =nullptr);
    ~Camera();
    void setUpCamera();
    void StartCamera();
    void showCamera();
    void stopCamera();
    // void resetCamera();
    // void setLastFrame(const cv::Mat& frame);
    bool takePicture();
    void onSnapButtonClicked();
    void setUpButtons();
    // void recordVideo(const std::string& filename, int durationSeconds = 10);
    void startRpiCamHello(const int& duration = 0);
    void timerForVideo();
    void showTimer();
    void hideTimer();
    std::string getCurrentTime();
    void insertToDB(const std::string& path,std::string type);
    void initDB();
    void displayCameraSettingbar();
    void showNotification(QWidget* parent, const QString& msg);



    Q_SIGNAL void recordFinished();
private:
    QLabel* _CameraLabel;
    QVBoxLayout* _mainlayout;
    CameraWorker* _camera;
    QHBoxLayout* _recordTimers;
    QWidget* _central_widget;

    QPushButton* fiveTimer;
    QPushButton* tenTimer;
    QPushButton* fifteenTimer;
    QPushButton* twentyTimer;
    std::filesystem::path _parentDir;
    std::filesystem::path _galleryPath;
    std::filesystem::path _albumDB;
    sqlite3* _db;
    QHBoxLayout* _buttonsLayout;
    QPushButton* _snapPictureBtn;
    QPushButton*_cameraBtn;
    QPushButton* _recordBtn;
    QPushButton* _settingBtn;

    QHBoxLayout* _settingsbarLayout;
    QVBoxLayout* _brightnessLayout;
    QVBoxLayout* _contrastLayout;
    QVBoxLayout* _saturationLayout;
    QPushButton* _brightnessUpButton;
    QPushButton* _brightnessDownButton;
    QPushButton* _contrastUpButton;
    QPushButton* _contrastDownButton;
    QPushButton* _saturationUpButton;
    QPushButton* _saturationDownButton;
    QLabel* _brightnessLabel;
    QLabel* _contrastLabel;
    QLabel* _saturationLabel;
    QWidget* _brightnessWidget;
    QWidget* _contrastWidget;
    QWidget* _saturationWidget;

    float BRIGHTNESSValue=0.0f;
    float CONTRASTValue=1.0f;
    float SATURATIONValue=1.0f;
    void cameraSettingsLayout();
    void setUpBrightnessButtons();
    void setUpContrastButtons();
    void setUpSaturationButtons();

    bool isCameraSettingDisplay = false;    

    bool _isStarted =false;
    bool _isSettingBarDisplay = false;
    QSettings* _settings;
    std::filesystem::path _captureDB= "";
   
     QString _btn_properties = R"(QPushButton {
                background-color: #000000;
                color: white;
                font-size: 20px;
                padding: 10px 20px;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #005f99;
})";
QString _btn_setting_properties = R"(QPushButton {
                background-color: #000000;
                color: white;
                font-size: 10px;
                padding: 10px 10px;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #005f99;
})";
};






// class ClickableLabel : public QLabel
// {
//     Q_OBJECT

// public:
//     explicit ClickableLabel(QWidget *parent = nullptr)
//         : QLabel(parent) {}

// signals:
//     void clicked();

// protected:
//     void mousePressEvent(QMouseEvent *event) override {
//         emit clicked();
//         QLabel::mousePressEvent(event); // optional, to keep default behavior
//     }
// };

