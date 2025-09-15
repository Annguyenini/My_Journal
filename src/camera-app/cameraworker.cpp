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
    frameSkip = 1;
    m_stopping = false;
    _requestPhoto = false;
    allocator = nullptr;
    stream = nullptr;
    _frameWidth = 2560;   // 2K resolution
    _frameHeight = 1440;
    _fps = 40;           // Target 40fps
}

CameraWorker::~CameraWorker() {
    stopCamera();
    if (cm) {
        cm->stop();
    }
}

void CameraWorker::requestComplete(libcamera::Request *request)
{
    if (request->status() != libcamera::Request::RequestComplete || m_stopping || !camera) {
        // Don't requeue if we're stopping or camera is invalid
        if (!m_stopping) {
            request->reuse(libcamera::Request::ReuseBuffers);
            if (camera) {
                camera->queueRequest(request);
            }
        }
        return;
    }

    for (auto &pair : request->buffers()) {
        const auto currentStream = pair.first;
        const auto &buffer = pair.second;
        
        if (!buffer || buffer->planes().empty()) {
            continue;
        }

        const auto &plane = buffer->planes()[0];
        int fd = plane.fd.get();
        size_t size = plane.length;
        
        // Validate buffer parameters before mapping
        if (fd < 0 || size == 0) {
            std::cerr << "Invalid buffer parameters: fd=" << fd << ", size=" << size << std::endl;
            continue;
        }
        
        void* mem = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
        if (mem == MAP_FAILED) {
            perror("mmap failed");
            continue;
        }
        
        // Get actual stream configuration
        if (!stream) {
            std::cerr << "Stream is null in requestComplete!" << std::endl;
            munmap(mem, size);
            continue;
        }
        
        // Use actual stream dimensions instead of hardcoded values
        const StreamConfiguration &config = stream->configuration();
        int width = config.size.width;
        int height = config.size.height;
        
        // Calculate stride based on pixel format (RGB888 = 3 bytes per pixel)
        int stride = width * 3;
        
        // Validate memory bounds
        size_t expected_size = height * stride;
        if (size < expected_size) {
            std::cerr << "Buffer size mismatch: expected " << expected_size << ", got " << size << std::endl;
            munmap(mem, size);
            continue;
        }
        
        // Create QImage with proper error checking
        QImage image;
        try {
            // Create image from mapped memory
            QImage temp_image(static_cast<const uchar*>(mem), width, height, stride, QImage::Format_BGR888);
            
            // IMPORTANT: Make a deep copy immediately to avoid using dangling pointer
            image = temp_image.copy();
            
            // Unmap immediately after copying
            munmap(mem, size);
            
            if (image.isNull()) {
                std::cerr << "Created QImage is null!" << std::endl;
                continue;
            }
            
            // Emit the COPIED image (not temp_image which is now invalid)
            Q_EMIT newFrameAvailable(image);
            
            // Handle photo capture
            if (_requestPhoto) {
                if (image.save(QString::fromStdString(_pictureFilename))) {
                    qDebug() << "Picture saved successfully: " << QString::fromStdString(_pictureFilename);
                } else {
                    qDebug() << "Failed to save picture: " << QString::fromStdString(_pictureFilename);
                }
                _requestPhoto = false;
            }
            
        } catch (const std::exception &e) {
            std::cerr << "Exception in image processing: " << e.what() << std::endl;
            munmap(mem, size);
            continue;
        }
        
        break; // Process only first valid buffer
    }
    
    // Immediately requeue for maximum throughput at 40fps
    if (!m_stopping && camera) {
        request->reuse(libcamera::Request::ReuseBuffers);
        camera->queueRequest(request);
    }
}

void CameraWorker::startCamera() {
    try {
        // Reset stopping flag
        m_stopping = false;
        
        if (!cm) {
            std::cerr << "CameraManager is null!" << std::endl;
            return;
        }
        
        cm->start();
        auto cameras = cm->cameras();
        if (cameras.empty()) {
            std::cout << "No cameras found!" << std::endl;
            return;
        }
        
        camera = cm->get(cameras[0]->id());
        if (!camera) {
            std::cout << "Failed to get camera!" << std::endl;
            return;
        }
        
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
        streamConfig.size.width = _frameWidth;
        streamConfig.size.height = _frameHeight;
        streamConfig.pixelFormat = libcamera::formats::RGB888;
        
        // Validate and adjust configuration if needed
        if (config->validate() == CameraConfiguration::Invalid) {
            std::cout << "Camera configuration invalid!" << std::endl;
            return;
        }
        
        // Log any adjustments made by validate()
        if (streamConfig.size.width != _frameWidth || streamConfig.size.height != _frameHeight) {
            std::cout << "Resolution adjusted by camera: " << streamConfig.size.width << "x" << streamConfig.size.height << std::endl;
        }
        
        if (camera->configure(config.get()) != 0) {
            std::cout << "Failed to configure camera!" << std::endl;
            return;
        }
        
        std::cout << "Camera pixel format: " << streamConfig.pixelFormat.toString() << std::endl;
        std::cout << "Camera resolution: " << streamConfig.size.width << "x" << streamConfig.size.height << std::endl;
        
        stream = streamConfig.stream();
        
        if (!stream) {
            std::cerr << "Stream pointer is null!" << std::endl;
            return;
        }
        
        // Clean up any existing allocator
        if (allocator) {
            delete allocator;
        }
        
        allocator = new FrameBufferAllocator(camera);
        if (allocator->allocate(stream) < 0) {
            std::cout << "Failed to allocate buffers!" << std::endl;
            delete allocator;
            allocator = nullptr;
            return;
        }
        
        std::cout << "Allocated " << allocator->buffers(stream).size() << " buffers for high-speed capture" << std::endl;
        
        // Clear any existing requests
        requests.clear();
        
        // Create requests - use all available buffers for better performance
        for (auto &buf : allocator->buffers(stream)) {
            auto req = camera->createRequest();
            if (!req) {
                std::cout << "Failed to create request!" << std::endl;
                continue;
            }
            
            if (req->addBuffer(stream, buf.get()) != 0) {
                std::cout << "Failed to add buffer to request!" << std::endl;
                continue;
            }
            requests.push_back(std::move(req));
        }
        
        if (requests.empty()) {
            std::cout << "No valid requests created!" << std::endl;
            return;
        }
        
        // Connect signal before starting
        camera->requestCompleted.connect(this, &CameraWorker::requestComplete);
        
        // Aggressive frame rate controls for 40fps
        camcontrols = std::make_unique<libcamera::ControlList>();
        
        // Calculate precise frame duration for 40fps
        int64_t frameDurationUs = 1000000 / _fps; // 25000 microseconds for 40fps
        
        std::cout << "Setting camera to " << _fps << " fps (frame duration: " << frameDurationUs << " μs)" << std::endl;
        
        // Primary method: Set exact frame duration limits
        camcontrols->set(controls::FrameDurationLimits, 
                        libcamera::Span<const std::int64_t, 2>({frameDurationUs, frameDurationUs}));
        
        // Additional controls for consistent high-speed performance
        try {
            // Disable auto exposure and white balance for consistent timing
            camcontrols->set(controls::AeEnable, false);
            camcontrols->set(controls::AwbEnable, false);
            
            // Set manual exposure time slightly shorter than frame duration
            int64_t exposureTimeUs = frameDurationUs - 3000; // Leave 3ms margin
            camcontrols->set(controls::ExposureTime, exposureTimeUs);
            
            // Set fixed gain
            camcontrols->set(controls::AnalogueGain, 2.0f);
            
            std::cout << "Applied manual exposure controls for consistent 40fps timing" << std::endl;
            
        } catch (const std::exception &e) {
            std::cout << "Some manual controls not available, using automatic settings: " << e.what() << std::endl;
        }
        
        // Note: FrameRate control not available in this libcamera version
        // Using FrameDurationLimits as primary method for 40fps control
        
        if (camera->start(camcontrols.get()) != 0) {
            std::cout << "Failed to start camera!" << std::endl;
            return;
        }
        
        // Queue all requests immediately for maximum throughput
        for (auto &req : requests) {
            if (camera->queueRequest(req.get()) != 0) {
                std::cout << "Failed to queue request!" << std::endl;
            }
        }
        
        std::cout << "Camera started successfully at " << _fps << " fps!" << std::endl;
        std::cout << "Target resolution: " << _frameWidth << "x" << _frameHeight << " (2K)" << std::endl;
        std::cout << "Buffer queue size: " << requests.size() << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "Exception in CameraWorker::startCamera: " << e.what() << std::endl;
        return; 
    }
}

void CameraWorker::requestPicture(std::string filename) {
    _requestPhoto = true;
    _pictureFilename = filename;
}

void CameraWorker::stopCamera() {
    if (!camera) {
        return;
    }
    
    std::cout << "Stopping camera..." << std::endl;
    
    // Set stopping flag to prevent new requests
    m_stopping = true;
    
    // Disconnect signal handler
    camera->requestCompleted.disconnect(this, &CameraWorker::requestComplete);
    
    // Stop camera
    camera->stop();
    
    // Clean up allocator
    if (allocator && stream) {
        allocator->free(stream);
        delete allocator;
        allocator = nullptr;
    }
    
    // Clear requests
    requests.clear();
    
    // Release camera
    camera->release();
    camera.reset();
    stream = nullptr;
    
    std::cout << "Camera stopped successfully" << std::endl;
}

#include "cameraworker.moc"