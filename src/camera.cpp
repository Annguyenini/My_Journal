#include "camera.h"
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <opencv2/imgproc.hpp>
CameraCallBack::CameraCallBack(QLabel* label, QObject* parent)
    : QObject(parent), _CameraLabel(label) {}
void CameraCallBack::hasFrame(const cv::Mat &frame, const libcamera::ControlList &) {
   if (_CameraLabel) {
        cv::Mat rgb=frame;
        

        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        // QImage qimg_copy = qimg.copy();  // Important!

        // QPixmap pixmap = QPixmap::fromImage(qimg);
        QPixmap pixmap = QPixmap::fromImage(qimg);
        Q_EMIT frameReady(pixmap);    
    }
}

Camera::Camera(QVBoxLayout* mainlayout) : _mainlayout(mainlayout), camera() {
}
void Camera::setUpCamera(){
    _CameraLabel = new QLabel();
    _CameraLabel->setObjectName("Camera-label");
    _CameraLabel->setMinimumSize(640, 480);  // set preferred size
    _CameraLabel->setAlignment(Qt::AlignCenter);
    _mainlayout->insertWidget(2, _CameraLabel);

    myCallback = new CameraCallBack(_CameraLabel,this);
    connect(myCallback, &CameraCallBack::frameReady, this, [this](const QPixmap &pixmap) {
    _CameraLabel->setPixmap(pixmap.scaled(_CameraLabel->size(), Qt::KeepAspectRatio));
    });
    camera.registerCallback(myCallback);
}


void Camera::StartCamera() {
    Libcam2OpenCVSettings setting;
    setting.width = 1920;          // Full HD width
    setting.height = 1080;         // Full HD height
    setting.framerate = 20;        // Lock it to 30 FPS for smoothness
    // setting.brightness = 0.5f;     // Medium brightness (adjust if needed)
    // // setting.contrast = 1.0f;       // Normal contrast
    // // setting.exposureTime = 10000;  // Lock exposure time (in microseconds), try 10ms as a start
    // setting.exposureValue = 0.0f;  // No extra EV adjustment
    // setting.saturation = 1.0f;     // Normal saturation
    // setting.lensPosition = -1.0f;  // Auto focus (or set manually if you want)
    camera.start(setting);
}
Camera::~Camera() {
    delete myCallback;
    delete _CameraLabel;
}