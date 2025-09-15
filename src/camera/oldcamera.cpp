#include "camera.h"
#include "gps.h"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QPixmap>
#include <QPushButton>
#include <QCoreApplication>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;
// Libcam2OpenCV Camera::camera;
CameraCallBack::CameraCallBack(QLabel* label, QObject* parent)
    : QObject(parent), _CameraLabel(label) {}
void CameraCallBack::hasFrame(const cv::Mat &frame, const libcamera::ControlList &) {
   if (_CameraLabel) {
        cv::Mat rgbMat;
        cv::cvtColor(frame, rgbMat, cv::COLOR_BGR2RGB);
        static_cast<Camera*>(parent())->setLastFrame(rgbMat);
        // cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);  // y,x coordinates
        // std::cout << "Pixel BGR: " << (int)pixel[0] << ", " << (int)pixel[1] << ", " << (int)pixel[2] << std::endl;


        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        // cv::Mat rgb;
        // cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);
        // QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
        // QImage qimg_copy = qimg.copy();  // Important!

        // QPixmap pixmap = QPixmap::fromImage(qimg);
        QPixmap pixmap = QPixmap::fromImage(qimg);
        Q_EMIT frameReady(pixmap);    
    }
}

Camera::Camera(QVBoxLayout* mainlayout) : _mainlayout(mainlayout) {
}

void Camera::setUpCamera(){
    qDebug() << "Setting up camera with settings:";
    _gpsObject = new GPSWorker();
    _CameraLabel = new QLabel();
    _albumObject = new AlbumWorker();
    camera =new Libcam2OpenCV();
    this->setUpAdjustmentBar();
    this->setupSetting();
    qDebug()<<"pass";
    _CameraLabel->setObjectName("Camera-label");
    // _CameraLabel->setStyleSheet("background-color: red;");

    _CameraLabel->setMinimumSize(200, 200);
    // _CameraLabel->setMinimumSize(800, 300);  // set preferred size
    _CameraLabel->setAlignment(Qt::AlignCenter);
    // _CameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // _mainlayout->insertWidget(1, _CameraLabel,1);
    myCallback = new CameraCallBack(_CameraLabel,this);
    
}

void Camera::setupSetting(){
    _setting.width = 1920;          // Full HD width
    _setting.height = 1080;         // Full HD height
    _setting.framerate = 60;        // Lock it to 30 FPS for smoothness
    _setting.brightness = 0;      // slightly brighter, softens shadows
    _setting.contrast = 0.9f;       // keep some punch
    _setting.saturation = 0.75f;    // a bit of color back
    _setting.exposureValue = 0.15f; // soft warm glow
    // _setting->contrast = 1.1f;      // more punch
    // _setting->saturation = 0.9f;    // slightly reduce to soften
    // _setting->exposureValue = 0.2; // soft glow
    
    // _setting->lensPosition = -1.0f; // keep autofocus or tweak manually if needed
}
void Camera::setUpAdjustmentBar(){
    _adjustmentBar = new QHBoxLayout();
    _brightnessSlider = new QSlider(Qt::Vertical);
    _brightnessSlider -> setMinimum(-50);
    _brightnessSlider -> setMaximum(200);
    _brightnessSlider -> setValue(100);
    _brightnessSlider -> setTickPosition(QSlider::TicksLeft);
    _brightnessSlider -> setTickInterval(30);

    _contrastSlider = new QSlider (Qt::Vertical);
    _contrastSlider->setMinimum(-50);
    _contrastSlider->setMaximum(200);
    _contrastSlider->setValue(90);
    _contrastSlider->setTickPosition(QSlider::TicksLeft);
    _contrastSlider->setTickInterval(30);

    _saturationSlider = new QSlider (Qt::Vertical);
    _saturationSlider->setMinimum(-50);
    _saturationSlider->setMaximum(200);
    _saturationSlider->setValue(100);
    _saturationSlider->setTickPosition(QSlider::TicksLeft);
    _saturationSlider->setTickInterval(30);

    _exposureSlider = new QSlider (Qt::Vertical);
    _exposureSlider->setMinimum(-50);
    _exposureSlider->setMaximum(200);
    _exposureSlider->setValue(100);
    _exposureSlider->setTickPosition(QSlider::TicksLeft);
    _exposureSlider->setTickInterval(30);

    _submitBtn = new QPushButton("Submit");
    _submitBtn->setStyleSheet(_btn_properties);

    _adjustmentBar->addWidget(_brightnessSlider);
    _adjustmentBar->addWidget(_contrastSlider);
    _adjustmentBar->addWidget(_saturationSlider);
    _adjustmentBar->addWidget(_exposureSlider);
    _adjustmentBar->addWidget(_submitBtn);
    connect(_submitBtn, &QPushButton::clicked, this, [this](){
        qDebug() << "Updating camera settings:";
        _setting.brightness = _brightnessSlider->value() / 100.0f;
        _setting.contrast = _contrastSlider->value() / 100.0f;
        _setting.saturation = _saturationSlider->value() / 100.0f;

        _setting.exposureValue = _exposureSlider->value() / 100.0f;
        qDebug() << "Camera settings updated:";
        qDebug() << "Brightness:" << _setting.brightness;
        qDebug() << "Contrast:" << _setting.contrast;
        qDebug() << "Saturation:" << _setting.saturation;
        qDebug() << "Exposure Value:" << _setting.exposureValue;
        qDebug() << "Adjustment bar hidden after settings update.";
        qDebug() << "Camera settings updated, emitting signal to MainWindow.";
        this->displayCameraSettingBar();
        qDebug() << "Camera settings emitted to MainWindow.";
        this->stopCamera();
        this->StartCamera();
    });

    
}
void Camera::displayCameraSettingBar(){
    if(isCameraSettingDisplay){
        qDebug() << "Hiding camera settings bar";
        _mainlayout->removeItem(_adjustmentBar);
        isCameraSettingDisplay = false;

    }
    else{
        qDebug() << "Displaying camera settings bar";
        _mainlayout->insertLayout(2, _adjustmentBar);
        isCameraSettingDisplay = true;
    }
}
static QMutex g_camMutex;

void Camera::StartCamera() {
    QMutexLocker lock(&g_camMutex);

    if (_isStarted) return;
    if (!_CameraLabel) return;


    // (Re)create camera if needed
    if (!camera) {
        camera = new Libcam2OpenCV();                // make sure Libcam2OpenCV ctor DOES NOT create a 2nd CameraManager
    }

    // Make sure no stale connections
    if (myCallback) {
        disconnect(myCallback, nullptr, this, nullptr);
    }

    connect(myCallback, &CameraCallBack::frameReady, this, [this](const QPixmap &pixmap) {
        if (!_CameraLabel) return;
        _CameraLabel->setPixmap(pixmap.scaled(_CameraLabel->size(), Qt::KeepAspectRatio));
    });

    camera->registerCallback(myCallback);

    qDebug() << "Starting camera...";
    camera->start(_setting);     // assume returns quickly; actual HW start may be async

    // Show UI only after start is requested
    if (_mainlayout && _CameraLabel && _CameraLabel->parent() == nullptr)
        _mainlayout->insertWidget(1, _CameraLabel, 1);
    if (_CameraLabel) _CameraLabel->show();

    _isStarted  = true;
}

void Camera::stopCamera() {
    QMutexLocker lock(&g_camMutex);

    if (!_isStarted) return;


    // 1) Disconnect + unregister callback FIRST so no more frames hit UI
    if (myCallback) {
        qDebug() << "Disconnect and unregister callback";
        disconnect(myCallback, nullptr, this, nullptr);
    }

    // 2) Hide UI
    if (_CameraLabel) _CameraLabel->hide();
    if (_mainlayout && _CameraLabel) _mainlayout->removeWidget(_CameraLabel);

    // 3) Ask camera to stop
    if (camera) {
        qDebug() << "Stopping camera...";
        camera->stop();
    }

    // 4) Bounded wait to let libcamera drain background work
    //    We don’t have a "stopped" signal, so pump events briefly.
    {
        QElapsedTimer t;
        t.start();
        // ~500ms usually enough; tweak if your hardware needs more
        while (t.elapsed() < 500) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        }
    }

    // 5) NOW it’s safe to destroy the camera instance
    if (camera) {
        qDebug() << "Deleting camera instance...";
        Libcam2OpenCV *old = camera;
        qDebug() << "Deleting camera instance on thread:" << QThread::currentThreadId();
        camera = nullptr;          // clear before delete so re-entrancy can’t use it
        qDebug() << "Deleting camera instance on thread:" << QThread::currentThreadId();
        delete old;                // destruct on the same thread it was created on
        qDebug() << "Camera deleted";
    }

    // 6) Eagerly recreate a fresh instance for the next Start (prevents ProcessManager reuse bugs)
    qDebug() << "Creating new camera instance";
    camera = new Libcam2OpenCV();

    _isStarted  = false;

    qDebug() << "Camera stopped/reset cleanly";
}

void Camera::setLastFrame(const cv::Mat& frame) {
        _lastFrame = frame.clone();
    }
bool Camera::takePicture() {
    std::string filename = _gpsObject->getCityImageDir().string()+"/"+_gpsObject->returnCurrentCity()+"_"+_gpsObject->getCurrentTime()+".jpg";
        qDebug()<<QString::fromStdString(filename);

        // std::string command ="rpicam-still --timeout 1 -o "+filename;
        // int ret = std::system(command.c_str());
        // if (ret != 0) {
        // std::cerr << "rpicamistill failed with code " << ret << std::endl;
    //     std::this_thread::sleep_for(std::chrono::microseconds(500));
        
    // }
        if (_lastFrame.empty()) return false;
        _albumObject -> insertToDB(filename,"photo");
        return cv::imwrite(filename, _lastFrame);
        // Q_EMIT recordFinished();
    }
void Camera::onSnapButtonClicked() {
    if (takePicture()) {
        qDebug() << "Picture saved!";
    } else {
        qDebug() << "No frame to save!";
    }
}
void Camera::startRpiCamHello(const int& duration) {
    Camera::hideTimer();
    std::string filename = _gpsObject->getCityImageDir().string()+"/"+_gpsObject->returnCurrentCity()+"_"+_gpsObject->getCurrentTime();
    std::string filenameH264 = filename+".h264";
    std::string filenameMP4 = filename+".mp4";
    std::string recordCommand = "rpicam-vid --width 1920 --height 1080 -t "+std::to_string(duration)+" -o "+filenameH264;
    // std::string recordCommand = "rpicam-vid --width 1920 --height 1080 --framerate 60 -t 5000 -o "+filename+".mp4 --inline --container mp4";
    std::cout << "Running: " << recordCommand << std::endl;
    int ret = std::system(recordCommand.c_str());
    if (ret != 0) {
        std::cerr << "rpicam-hello failed with code " << ret << std::endl;
    }
    std::string convertCommand ="ffmpeg -i "+ filenameH264 +" -c copy "+ filenameMP4;
    int conv = std::system(convertCommand.c_str());
    if (conv != 0){
        std::cerr << "Convert failed: " << ret << std::endl;

    }
    std::string removeCommand = "rm -rf "+filenameH264;
    int rm = std::system(removeCommand.c_str());
    if(rm !=0 ){
        std::cerr << "rm fail" << ret << std::endl;

    }
    this->hideTimer();
    _albumObject -> insertToDB(filenameMP4,"video");
    Q_EMIT recordFinished();
}
void Camera::timerForVideo(){
    _recordTimers = new QHBoxLayout();
    fiveTimer = new QPushButton("5");
    tenTimer = new QPushButton("10");
    fifteenTimer = new QPushButton("15");
    twentyTimer = new QPushButton("20");
    for (QPushButton* btn : {fiveTimer,tenTimer,fifteenTimer,twentyTimer}){
        btn ->setStyleSheet(_btn_properties);
        _recordTimers->addWidget(btn);
        
    }
    connect(fiveTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(5*1000);
        });
    connect(tenTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(10*1000);
        });

    connect(fifteenTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(15*1000);
        });
    connect(twentyTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(20*1000);
        });
    Camera::showTimer();
}
void Camera::showTimer(){
    if (!_recordTimers) return;
        for (int i = 0; i < _recordTimers->count(); ++i) {
            QWidget* w = _recordTimers->itemAt(i)->widget();
            if (w) w->show();
        }
    _mainlayout->insertLayout(2,_recordTimers);
    
}
void Camera::hideTimer(){
    
    if (!_recordTimers) return;
        for (int i = 0; i < _recordTimers->count(); ++i) {
            QWidget* w = _recordTimers->itemAt(i)->widget();
            if (w) w->hide();
        }
    _mainlayout->removeItem(_recordTimers);
}
// void Camera::recordVideo(const std::string& filename, int durationSeconds) {
//     VideoCapture cap(1);
//     if (!cap.isOpened()) {
//         cerr << "ERROR! Unable to open camera\n";
//         return;
//     }

//     Mat frame;
//     cap >> frame;
//     if (frame.empty()) {
//         cerr << "ERROR! Blank frame grabbed\n";
//         return;
//     }

//     bool isColor = (frame.type() == CV_8UC3);
//     Size frameSize = frame.size();

//     int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
//     double fps = 5.0;  // Your target FPS

//     VideoWriter writer(filename, codec, fps, frameSize, isColor);
//     if (!writer.isOpened()) {
//         cerr << "ERROR! Could not open output video file for write\n";
//         return;
//     }

//     cout << "Recording video at " << fps << " FPS to " << filename << " for " << durationSeconds << " seconds\n";

//     auto start = chrono::steady_clock::now();
//     auto frameDuration = chrono::milliseconds(static_cast<int>(1000.0 / fps));

//     while (true) {
//         auto loopStart = chrono::steady_clock::now();

//         if (!cap.read(frame) || frame.empty()) {
//             cerr << "ERROR! Blank frame grabbed\n";
//             break;
//         }

//         writer.write(frame);
//         imshow("Live", frame);

//         if (waitKey(1) >= 0) {
//             cout << "Recording stopped by user\n";
//             break;
//         }

//         auto now = chrono::steady_clock::now();
//         auto elapsed = chrono::duration_cast<chrono::seconds>(now - start).count();
//         if (elapsed >= durationSeconds) {
//             cout << "Recording time completed\n";
//             break;
//         }

//         // Make sure we wait the right amount of time for consistent FPS
//         auto loopEnd = chrono::steady_clock::now();
//         auto elapsedLoop = chrono::duration_cast<chrono::milliseconds>(loopEnd - loopStart);
//         if (elapsedLoop < frameDuration) {
//             std::this_thread::sleep_for(frameDuration - elapsedLoop);
//         }
//     }
//     destroyAllWindows();
// }





Camera::~Camera() {
    delete myCallback;
    delete _CameraLabel;
}





