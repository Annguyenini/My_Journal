// #pragma once
// #include <QWidget>
// #include <QLabel>
// #include <QTimer>
// #include <QImage>
// // #undef signals
// // #undef slots
// // #undef emit
// // #undef foreach
// #include <memory>
// #include <libcamera/camera_manager.h>
// #include <libcamera/camera.h>

// class CameraWidget : public QWidget
// {
//     Q_OBJECT  // MUST have this

// public:
//     explicit CameraWidget(QWidget *parent = nullptr);
//     ~CameraWidget();

//     void startCamera();
//     void stopCamera();

// private slots:
//     void grabFrame();   // now properly recognized as a slot

// private:
//     QLabel* m_cameraLabel;
//     QTimer* m_timer;

//     std::unique_ptr<libcamera::CameraManager> m_cameraManager;
//     libcamera::Camera* m_camera;  // raw pointer, assign via .get()
//     bool m_cameraStarted;
// };
// CameraView.h
#pragma once
#include <QObject>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <memory>
#include <thread>
#include <sys/mman.h>
#include <opencv2/opencv.hpp>
#undef signals
#undef slots
#undef emit
#undef foreach
#include <libcamera/libcamera.h>

// std::shared_ptr<libcamera::Camera> camera;

class CameraWorker : public QObject {
    Q_OBJECT

public:
    explicit CameraWorker(QObject *parent = nullptr);
    ~CameraWorker();

public Q_SLOTS:
    void startCamera();
    void stopCamera();

Q_SIGNALS:
    void newFrameAvailable(const QImage &image);  // Signal to emit frames

private:
    void requestComplete(libcamera::Request *request);
    int frameCounter = 0;
    int frameSkip = 1;
    
    std::unique_ptr<libcamera::CameraManager> cm;
    std::shared_ptr<libcamera::Camera> camera;
    std::unique_ptr<libcamera::ControlList> camcontrols;

    libcamera::FrameBufferAllocator *allocator = nullptr;
    std::vector<std::unique_ptr<libcamera::Request>> requests;
    libcamera::Stream *stream = nullptr;
};

class TCamera : public QObject {
    Q_OBJECT

public:
    TCamera(QVBoxLayout* mainlayout);
    void startCamera();

private:
    QVBoxLayout* _mainlayout;
    QLabel* Tcamera;
    CameraWorker* _cameraWorker;
};