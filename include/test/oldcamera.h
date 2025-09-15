#ifndef CAMERA_H
#define CAMERA_H
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

class CameraCallBack :public QObject, public Libcam2OpenCV::Callback {
    Q_OBJECT
public:
    explicit CameraCallBack(QLabel* label, QObject* parent = nullptr);
    void hasFrame(const cv::Mat &frame, const libcamera::ControlList &) override;
    Q_SIGNAL void frameReady(const QPixmap &pixmap);  
private:
    QLabel* _CameraLabel;

};

class Camera:public QObject {
    Q_OBJECT
public:
    explicit  Camera(QVBoxLayout* mainlayout);
    ~Camera();
    void setUpCamera();
    void StartCamera();
    void showCamera();
    void stopCamera();
    // void resetCamera();
    void setLastFrame(const cv::Mat& frame);
    bool takePicture();
    void onSnapButtonClicked();
    // void recordVideo(const std::string& filename, int durationSeconds = 10);
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
    cv::Mat _lastFrame;
    CameraCallBack* myCallback;
    QHBoxLayout* _recordTimers;
    GPSWorker* _gpsObject;
    QPushButton* fiveTimer;
    QPushButton* tenTimer;
    QPushButton* fifteenTimer;
    QPushButton* twentyTimer;
    QHBoxLayout* _adjustmentBar;
    QSlider* _brightnessSlider;
    QSlider* _contrastSlider;
    QSlider* _saturationSlider;
    QSlider* _exposureSlider;
    QPushButton* _submitBtn;
    Libcam2OpenCVSettings _setting;
    std::unique_ptr<libcamera::CameraManager> cm;

    bool isCameraSettingDisplay = false;
    // Buttons* _button;
    
    bool _isStarted =false;

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

#endif // CAMERA_H