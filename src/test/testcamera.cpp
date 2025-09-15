// #include "testcamera.h"
// #include <QVBoxLayout>
// #include <QDebug>

// CameraWidget::CameraWidget(QWidget *parent)
//     : QWidget(parent),
//       m_cameraLabel(new QLabel(this)),
//       m_timer(new QTimer(this)),
//       m_cameraManager(std::make_unique<libcamera::CameraManager>()),
//       m_camera(nullptr),
//       m_cameraStarted(false)
// {
//     QVBoxLayout* layout = new QVBoxLayout(this);
//     layout->addWidget(m_cameraLabel);
//     setLayout(layout);

//     connect(m_timer, &QTimer::timeout, this, &CameraWidget::grabFrame);
// }

// CameraWidget::~CameraWidget()
// {
//     stopCamera();
// }

// void CameraWidget::startCamera()
// {
//     if (m_cameraStarted) return;

//     m_cameraManager->start();

//     if (m_cameraManager->cameras().empty()) {
//         qWarning() << "No cameras found!";
//         return;
//     }

//     m_camera = m_cameraManager->cameras()[0].get(); // <- use .get()
//     if (m_camera->acquire() != 0) {
//         qWarning() << "Failed to acquire camera!";
//         m_camera = nullptr;
//         return;
//     }

//     m_cameraStarted = true;
//     m_timer->start(33); // ~30fps
// }

// void CameraWidget::stopCamera()
// {
//     if (!m_cameraStarted) return;

//     m_timer->stop();

//     if (m_camera) {
//         m_camera->release();
//         m_camera = nullptr;
//     }

//     m_cameraManager->stop();
//     m_cameraStarted = false;
// }

// void CameraWidget::grabFrame()
// {
//     if (!m_camera) return;

//     // Minimal: just display text as placeholder
//     // To show actual frames, need to map libcamera buffers to QImage
//     m_cameraLabel->setText("Frame captured"); 
// }
// cameraworker.cpp
// cameraworker.cpp
#include "testcamera.h"
#include <thread>
#include <iostream>
#include <sys/mman.h>
#include <QImage>
#include <QPixmap>

using namespace libcamera;
using namespace std::chrono_literals;

CameraWorker::CameraWorker(QObject *parent) : QObject(parent) {
    cm = std::make_unique<CameraManager>();
    cm->start();
    frameCounter = 0;
    frameSkip = 1; // Skip every other frame for 30fps -> 15fps effective display rate
}

CameraWorker::~CameraWorker() {
    stopCamera();
    if (cm) {
        cm->stop();
    }
}

void CameraWorker::requestComplete(libcamera::Request *request)
{
    if (request->status() == libcamera::Request::RequestComplete) {
        
        // CPU OPTIMIZATION 1: Frame skipping to reduce processing load
        // frameCounter++;
        // if (frameCounter % (frameSkip + 1) != 0) {
        //     // Skip processing this frame entirely
        //     request->reuse(libcamera::Request::ReuseBuffers);
        //     camera->queueRequest(request);
        //     return;
        // }
        
        for (auto &pair : request->buffers()) {
            const auto &buffer = pair.second;
            
           if (!buffer->planes().empty()) {
                const auto &plane = buffer->planes()[0];
                int fd = plane.fd.get();
                size_t size = plane.length;
                
                void* mem = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
                if (mem == MAP_FAILED) {
                    perror("mmap failed");
                    continue;
                }
                
                // CPU OPTIMIZATION 2: Use smaller processing resolution
                static constexpr int width = 1920;
                static constexpr int height = 1080;
                static constexpr int stride = width * 4;
                
                // CPU OPTIMIZATION 3: Create QImage with minimal copying
                QImage image(static_cast<const uchar*>(mem), width, height, stride, QImage::Format_RGB32);
                
                // CPU OPTIMIZATION 4: Scale down during copy to reduce memory and processing
                // Scale to 50% resolution (960x540) to reduce CPU load significantly
                QImage scaledImage = image.scaled(960, 540, Qt::IgnoreAspectRatio, Qt::FastTransformation);
                
                munmap(mem, size);
                
                // CPU OPTIMIZATION 5: Emit QImage directly instead of converting to QPixmap
                Q_EMIT newFrameAvailable(scaledImage);
                break;
            }
        }
    }
    
    // Requeue the request
    request->reuse(libcamera::Request::ReuseBuffers);
    camera->queueRequest(request);
}

void CameraWorker::startCamera() {
    auto cameras = cm->cameras();
    if (cameras.empty()) {
        std::cout << "No cameras found!" << std::endl;
        return;
    }
    
    camera = cm->get(cameras[0]->id());
    if (camera->acquire() != 0) {
        std::cout << "Failed to acquire camera!" << std::endl;
        return;
    }
    
    auto config = camera->generateConfiguration({ StreamRole::Viewfinder });
    if (!config) {
        std::cout << "Failed to generate configuration!" << std::endl;
        return;
    }
    
    StreamConfiguration &streamConfig = config->at(0);
    
    // CPU OPTIMIZATION 6: Use lower resolution if acceptable
    // streamConfig.size.width = 1280;  // 720p instead of 1080p
    // streamConfig.size.height = 720;
    streamConfig.size.width = 1920;
    streamConfig.size.height = 1080;
    
    // CPU OPTIMIZATION 7: Use more efficient pixel format if camera supports it
    // streamConfig.pixelFormat = libcamera::formats::YUYV; // More efficient than RGB
    
    if (config->validate() == CameraConfiguration::Invalid) {
        std::cout << "Camera configuration invalid!" << std::endl;
        return;
    }
    
    if (camera->configure(config.get()) != 0) {
        std::cout << "Failed to configure camera!" << std::endl;
        return;
    }
    
    std::cout << "Camera pixel format: " << streamConfig.pixelFormat.toString() << std::endl;
    std::cout << "Camera resolution: " << streamConfig.size.width << "x" << streamConfig.size.height << std::endl;
    
    stream = streamConfig.stream();
    
    allocator = new FrameBufferAllocator(camera);
    if (allocator->allocate(stream) < 0) {
        std::cout << "Failed to allocate buffers!" << std::endl;
        delete allocator;
        allocator = nullptr;
        return;
    }
    
    requests.clear();
    for (auto &buf : allocator->buffers(stream)) {
        auto req = camera->createRequest();
        if (req->addBuffer(stream, buf.get()) != 0) {
            std::cout << "Failed to add buffer to request!" << std::endl;
            continue;
        }
        requests.push_back(std::move(req));
    }
    
    camera->requestCompleted.connect(this, &CameraWorker::requestComplete);
    
    // Set to 30fps with frame duration limits
    camcontrols = std::unique_ptr<libcamera::ControlList>(new libcamera::ControlList());
    camcontrols->set(controls::FrameDurationLimits, libcamera::Span<const std::int64_t, 2>({25000, 25000}));
    
    if (camera->start(camcontrols.get()) != 0) {
        std::cout << "Failed to start camera!" << std::endl;
        return;
    }
    
    for (auto &req : requests) {
        camera->queueRequest(req.get());
    }
    
    std::cout << "Camera started successfully with CPU optimizations!" << std::endl;
}

void CameraWorker::stopCamera() {
    if (camera) {
        camera->stop();
        camera->requestCompleted.disconnect(this, &CameraWorker::requestComplete);
        camera->release();
        camera.reset();
    }
    
    if (allocator) {
        delete allocator;
        allocator = nullptr;
    }
    
    requests.clear();
}

TCamera::TCamera(QVBoxLayout* mainlayout) : _mainlayout(mainlayout) {
    Tcamera = new QLabel();
    Tcamera->setAlignment(Qt::AlignCenter);
    Tcamera->setMinimumSize(640, 480);
    Tcamera->setStyleSheet("border: 1px solid black;");
    _mainlayout->addWidget(Tcamera);
    
    _cameraWorker = new CameraWorker(this);
}

void TCamera::startCamera() {
    // CPU OPTIMIZATION 8: Direct QImage to QPixmap conversion without extra scaling
    connect(_cameraWorker, &CameraWorker::newFrameAvailable, this, 
            [this](const QImage &img) {
                if (!img.isNull()) {
                    // Convert to QPixmap only once and use KeepAspectRatio with FastTransformation
                    QPixmap pixmap = QPixmap::fromImage(img);
                    Tcamera->setPixmap(pixmap.scaled(
                        Tcamera->size(), Qt::KeepAspectRatio, Qt::FastTransformation
                    ));
                }
            });
    
    _cameraWorker->startCamera();
}

// Don't forget to add this to the end of your .cpp file for Qt's MOC system
#include "testcamera.moc"