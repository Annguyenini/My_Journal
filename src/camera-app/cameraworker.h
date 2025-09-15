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
    void requestPicture(std::string filename);
    void setBrightness (float value);
    void setContrast (float value);
    void setSaturation (float value);
    void setISO(float value);
    void setExposure(float value);
    void setRedGain(float rGain);
    void setBlueGain(float bGain);
    void applySettings();

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
    bool m_stopping = false;
    bool _requestPhoto =false;
    std::string _pictureFilename;
    std::unique_ptr<libcamera::CameraManager> cm;
    std::shared_ptr<libcamera::Camera> camera;
    std::unique_ptr<libcamera::ControlList> camcontrols;
    libcamera::FrameBufferAllocator *allocator = nullptr;
    std::vector<std::unique_ptr<libcamera::Request>> requests;
    libcamera::Stream *stream = nullptr;
    // libcamera::Stream *stillStream = nullptr; // For still capture
    unsigned int _frameWidth = 2560;  // Default width
    unsigned int _frameHeight = 1440; // Default height
    unsigned int _frameWidthStill = 2560; // Default height
    unsigned int _frameHeightStill = 1440; // Default width for still capture
    unsigned int _fps = 40;

    // Camera settings    
    float BRIGHTNESS = 0.0f;
    float CONTRAST = 0.0f;
    float SATURATION = 0.0f;
    float ISO = 100.0f;
    float EXPOSURE = 10000;
    float R_GAIN = 1.0f;
    float B_GAIN = 1.0f;

    
};

// class TCamera : public QObject {
//     Q_OBJECT

// public:
//     TCamera(QVBoxLayout* mainlayout);
//     void startCamera();

// private:
//     QVBoxLayout* _mainlayout;
//     QLabel* Tcamera;
//     CameraWorker* _cameraWorker;
// };