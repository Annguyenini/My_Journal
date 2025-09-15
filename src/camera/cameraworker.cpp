#include "cameraworker.h"
#include <thread>
#include <iostream>
#include <sys/mman.h>
#include <QImage>
#include <QPixmap>

using namespace libcamera;
using namespace std::chrono_literals;

CameraWorker::CameraWorker(QObject *parent) : QObject(parent) {
    cm = std::make_unique<CameraManager>();
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
        if (!camera||m_stopping){
            std::cout<<"sto11111111111111111111111111111111111ppppppppp";
                    request->reuse(libcamera::Request::ReuseBuffers);

            return;
        }
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
                QImage image(static_cast<const uchar*>(mem), width, height, stride, QImage::Format_RGB32); //DANGER do not  use directly, it will crash the shit out
                
                // CPU OPTIMIZATION 4: Scale down during copy to reduce memory and processing
                // Scale to 50% resolution (960x540) to reduce CPU load significantly
                // QImage scaledImage = image.scaled(960, 540, Qt::IgnoreAspectRatio, Qt::FastTransformation);
                QImage imageCopy = image.copy();  

                munmap(mem, size);
                
                // CPU OPTIMIZATION 5: Emit QImage directly instead of converting to QPixmap
                Q_EMIT newFrameAvailable(imageCopy);
                
                if (_requestPhoto){
                    imageCopy.save(QString::fromStdString(_pictureFilename));
                    _requestPhoto = false;
                    qDebug()<<"Complete taking picture";
                }
                break;
            }
        }
    }
    
    // Requeue the request
    request->reuse(libcamera::Request::ReuseBuffers);
    if (!m_stopping) {  // <-- only queue if not stopping
        camera->queueRequest(request);
    }
}

void CameraWorker::startCamera() {
    try{

    
        cm->start();
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
        std::cout << "Camera resolution: " << streamConfig.size.width << "x" << streamConfig.size.height <<"aasasa"<< std::endl;
        std::cout<<"dppppp";
        stream = streamConfig.stream();
        if (!stream) {
        std::cerr << "Stream pointer is null!" << std::endl;
        return ;
    }
        std::cout<<"pass1";
        allocator = new FrameBufferAllocator(camera);
        std::cout<<"pass2";
        if (allocator->allocate(stream) < 0) {
            std::cout << "Failed to allocate buffers!" << std::endl;
            delete allocator;
            allocator = nullptr;
            return;
        }
        std::cout<<"pass3";
        requests.clear();
        std::cout<<"pass4";
        for (auto &buf : allocator->buffers(stream)) {
            std::cout<<"pass5";
            auto req = camera->createRequest();
            std::cout<<"pass6";
            if (req->addBuffer(stream, buf.get()) != 0) {
                std::cout << "Failed to add buffer to request!" << std::endl;
                continue;
            }
            requests.push_back(std::move(req));
        }
        std::cout<<"pass7";
        camera->requestCompleted.connect(this, &CameraWorker::requestComplete);
        std::cout<<"pass8";
        // Set to 30fps with frame duration limits
        camcontrols = std::unique_ptr<libcamera::ControlList>(new libcamera::ControlList());
        std::cout<<"pass9";
        camcontrols->set(controls::FrameDurationLimits, libcamera::Span<const std::int64_t, 2>({33333, 33333}));
        std::cout<<"pass10";
        if (camera->start(camcontrols.get()) != 0) {
            std::cout << "Failed to start camera!" << std::endl;
            return;
        }
        std::cout<<"pass11";
        for (auto &req : requests) {
            camera->queueRequest(req.get());
        }
        std::cout<<"pass12";
        std::cout << "Camera started successfully with CPU optimizations!" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Exception in CameraWorker::startCamera: " << e.what() << std::endl;
        return; 
    }
}
void CameraWorker::requestPicture(std::string filename){
    _requestPhoto = true;
    _pictureFilename = filename;
}
void CameraWorker::stopCamera() {
    if (camera) {
        m_stopping = true;          // <- prevent re-queue
        camera->requestCompleted.disconnect(this, &CameraWorker::requestComplete);

        std::cout<<"acttualy stop";
        camera->stop();
        
        allocator->free(stream);
        delete allocator;
        allocator =nullptr;
        camera->release();
        camera.reset();
        stream =nullptr;
        // cm->stop();
        m_stopping = false; 
    }
    
    
    
}
#include "cameraworker.moc"