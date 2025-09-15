#include "mainwindow.h"
#include "configure.h"

#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // QSerialPort serial;
    MainWindow w;
    // StatusBar s;
    // s.SetUpStatusBar();
    // GPS g;
    // g.show();
    // GPSWorker*gps =new GPSWorker();
    // gps->startReadingFromGps();
    w.show();
    w.showFullScreen();
    return a.exec();
}



// #include <iomanip>
// #include <iostream>
// #include <memory>
// #include <thread>

// #include <libcamera/libcamera.h>

// using namespace libcamera;
// using namespace std::chrono_literals;
// static std::shared_ptr<Camera> camera;
// static void requestComplete(Request *request)
// {
//     if (request->status() == Request::RequestCancelled)return;
//     const std::map<const Stream *, FrameBuffer *> &buffers = request->buffers();
//     for (auto bufferPair : buffers) {
//         FrameBuffer *buffer = bufferPair.second;
//         const FrameMetadata &metadata = buffer->metadata();


//         std::cout << " seq: " << std::setw(6) << std::setfill('0') << metadata.sequence << " bytesused: ";

//         unsigned int nplane = 0;
//         for (const FrameMetadata::Plane &plane : metadata.planes())
//         {
//             std::cout << plane.bytesused;
//             if (++nplane < metadata.planes().size()) std::cout << "/";
//  std::cout << "Plane " << i << " length: " << plane.length << std::endl; // total buffer length
//     std::cout << "Plane " << i << " mem: " << plane.m.fd << std::endl; // memory fd (if dmabuf)        }

//         std::cout << std::endl;
//     }
// }

//     request->reuse(Request::ReuseBuffers);
//     camera->queueRequest(request);
// }
// int main()
// {
//     std::unique_ptr<CameraManager> cm = std::make_unique<CameraManager>();
//     cm->start();
//     for (auto const &camera : cm->cameras())
//     std::cout << camera->id() << std::endl;
//     auto cameras = cm->cameras();
//     if (cameras.empty()) {
//         std::cout << "No cameras were identified on the system."
//                 << std::endl;
//         cm->stop();
//         return EXIT_FAILURE;
//     }
//     std::string cameraId = cameras[0]->id(); //// CAMERA INDEX

//     camera = cm->get(cameraId);
//     camera->acquire();
//     std::unique_ptr<CameraConfiguration> config = camera->generateConfiguration( { StreamRole::Viewfinder } );//camera configuration 
//     StreamConfiguration &streamConfig = config->at(0); //stream configuration
//     std::cout << "Default viewfinder configuration is: " << streamConfig.toString() << std::endl;
//     ///
//     streamConfig.size.width = 640;
//     streamConfig.size.height = 480;
//     ///CONGIF
//     config->validate();
//     std::cout << "Validated viewfinder configuration is: " << streamConfig.toString() << std::endl;
//     camera->configure(config.get());
//     FrameBufferAllocator *allocator = new FrameBufferAllocator(camera);/// create a allocator for camera
//     for (StreamConfiguration &cfg : *config) {
//         int ret = allocator->allocate(cfg.stream()); //reserve memory
//         if (ret < 0) {
//             std::cerr << "Can't allocate buffers" << std::endl;
//             return -ENOMEM;
//         }

//         size_t allocated = allocator->buffers(cfg.stream()).size();
//         std::cout << "Allocated " << allocated << " buffers for stream" << std::endl;
//     }

//     //libcamera process request per frame
//     Stream *stream = streamConfig.stream(); //create stream 
//     const std::vector<std::unique_ptr<FrameBuffer>> &buffers = allocator->buffers(stream); //assosiate stream to each buffer
//     std::vector<std::unique_ptr<Request>> requests;//declear the request vector

//     for (unsigned int i = 0; i < buffers.size(); ++i) { //loop through the buffer size
//         std::unique_ptr<Request> request = camera->createRequest(); ///create request for each buffer/frame
//         if (!request)
//         {
//             std::cerr << "Can't create request" << std::endl;
//             return -ENOMEM;
//         }

//         const std::unique_ptr<FrameBuffer> &buffer = buffers[i];
//         int ret = request->addBuffer(stream, buffer.get());
//         if (ret < 0)
//         {
//             std::cerr << "Can't set buffer for request"
//                 << std::endl;
//             return ret;
//         }

//         requests.push_back(std::move(request));
//     }
//     camera->requestCompleted.connect(requestComplete);
//     camera->start();
//     for (std::unique_ptr<Request> &request : requests)
//     camera->queueRequest(request.get());
//     std::this_thread::sleep_for(3000ms);
//     camera->stop();                // Stop the camera from producing more frames
//     camera->requestCompleted.disconnect(); // Disconnect your callback just in case

//     // Make sure all requests are finished (optional but safer)
//     std::this_thread::sleep_for(50ms); // tiny delay to flush pending callbacks

//     allocator->free(stream);        // Free buffers associated with the stream
//     delete allocator;               // Delete the allocator

//     camera->release();              // Release camera resources
//     camera.reset();                 // Reset the shared_ptr
//     cm->stop();                     // Stop the camera manager

//         // Code to follow

//     return 0;
// }
