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
Camera::Camera(QVBoxLayout* mainlayout) : _mainlayout(mainlayout) {
}

void Camera::setUpCamera(){
    qDebug() << "Setting up camera with settings:";
    // _gpsObject = new GPSWorker();
    _CameraLabel = new QLabel();
    _albumObject = new AlbumWorker();
    _camera =new CameraWorker();
    qDebug()<<"pass";
    _CameraLabel->setObjectName("Camera-label");
    // _CameraLabel->setStyleSheet("background-color: red;");

    _CameraLabel->setMinimumSize(200, 200);
    // _CameraLabel->setMinimumSize(800, 300);  // set preferred size
    _CameraLabel->setAlignment(Qt::AlignCenter);
    // _CameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // _mainlayout->insertWidget(1, _CameraLabel,1);
    
}



static QMutex g_camMutex;

void Camera::StartCamera() {
    QMutexLocker lock(&g_camMutex);

    if (_isStarted) return;
    if (!_CameraLabel) return;


  

    // Make sure no stale connections

    connect(_camera, &CameraWorker::newFrameAvailable, this, [this](const QImage &image) {
        QPixmap pixmap = QPixmap::fromImage(image);
        if (!_CameraLabel) return;
        _CameraLabel->setPixmap(pixmap.scaled(_CameraLabel->size(), Qt::KeepAspectRatio));
    });


    qDebug() << "Starting camera...";
    _camera->startCamera();     // assume returns quickly; actual HW start may be async

    // Show UI only after start is requested
    if (_mainlayout && _CameraLabel && _CameraLabel->parent() == nullptr)
        _mainlayout->insertWidget(1, _CameraLabel, 1);
    if (_CameraLabel) _CameraLabel->show();

    _isStarted  = true;
}

void Camera::stopCamera() {
    QMutexLocker lock(&g_camMutex);

    if (!_isStarted) return;


    

    // 2) Hide UI
    if (_CameraLabel) _CameraLabel->hide();
    if (_mainlayout && _CameraLabel) _mainlayout->removeWidget(_CameraLabel);

    // 3) Ask camera to stop
    if (_camera) {
        qDebug() << "Stopping camera...";
        _camera->stopCamera();
    }

    
    _isStarted  = false;

    qDebug() << "Camera stopped/reset cleanly";
}


bool Camera::takePicture() {
    std::string filename = _gpsObject->getCityImageDir().string()+"/"+_gpsObject->returnCurrentCity()+"_"+_gpsObject->getCurrentTime()+".jpg";
        qDebug()<<QString::fromStdString(filename);
        _camera->requestPicture(filename);
        _albumObject -> insertToDB(filename,"photo");
        return true;
}
void Camera::onSnapButtonClicked() {
    if (takePicture()) {
        qDebug() << "Picture saved!";
    } else {
        qDebug() << "No frame to save!";
    }
}
void Camera::startRpiCamHello(const int& duration) {
    _camera->stopCamera();
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
    // Q_EMIT recordFinished();
    _camera->startCamera();  // Restart camera after recording
    qDebug() << "Recording finished, camera restarted";
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
    // delete myCallback;
}





