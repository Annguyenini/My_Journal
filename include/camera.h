#ifndef CAMERA_H
#define CAMERA_H
#include <QObject>
#include <QLabel>
#include <QVBoxLayout>
#include "libcam2opencv.h"

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
    Camera(QVBoxLayout* mainlayout);
    ~Camera();
    void setUpCamera();
    void StartCamera();

private:
    QLabel* _CameraLabel;
    QVBoxLayout* _mainlayout;
    Libcam2OpenCV camera;
    CameraCallBack* myCallback;
};

#endif // CAMERA_H
