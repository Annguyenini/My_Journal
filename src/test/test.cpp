#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSerialPort serial;
    MainWindow w;
    w.show();
    
    // Initialize the GPSWorker
    serial.setPortName("/dev/serial0");
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    if (!serial.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open serial port!";
        return -1;
    }
    QObject::connect(&serial, &QSerialPort::readyRead, [&]() {
        QByteArray data = serial.readAll();
        QString line = QString::fromUtf8(data);
        QStringList parts = line.split(',');
        if (parts[0] == "$GPRMC") {
            float lat = parts[3].toFloat();
            float lng = parts[5].toFloat();
            qDebug() << "Latitude:" << lat << "Longitude:" << lng;
        } else if (parts[0] == "$GPVTG") {
            float speed = parts[7].toFloat();
            qDebug() << "Speed:" << speed;
        } else {
            qDebug() << "Fail to read data or there is no good data!";
        }
    });

    return a.exec();
}


#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create SDL window
    SDL_Window *window = SDL_CreateWindow("Camera Preview",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1920, 1080, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL create window error: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "SDL create renderer error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Build GStreamer pipeline with appsink
    const char *pipeline_desc =
    "libcamerasrc ! video/x-raw,width=1920,height=1080,framerate=20/1,format=NV12 ! "
    "videoconvert ! "
    "xvimagesink name=videosink sync=false";


    GError *error = nullptr;
    GstElement *pipeline = gst_parse_launch(pipeline_desc, &error);
    if (!pipeline) {
        std::cerr << "Failed to create pipeline: " << (error ? error->message : "unknown") << "\n";
        if (error) g_error_free(error);
        return -1;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    gst_app_sink_set_emit_signals((GstAppSink*)appsink, true);
    gst_app_sink_set_drop((GstAppSink*)appsink, true);
    gst_app_sink_set_max_buffers((GstAppSink*)appsink, 1);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        // Pull sample from appsink
        GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(appsink), 1000000);
        if (sample) {
            GstBuffer *buffer = gst_sample_get_buffer(sample);
            GstCaps *caps = gst_sample_get_caps(sample);
            GstStructure *s = gst_caps_get_structure(caps, 0);
            int width, height;
            gst_structure_get_int(s, "width", &width);
            gst_structure_get_int(s, "height", &height);

            GstMapInfo map;
            gst_buffer_map(buffer, &map, GST_MAP_READ);

            // Create/update SDL texture
            static SDL_Texture *texture = nullptr;
            if (!texture) {
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                    SDL_TEXTUREACCESS_STREAMING, width, height);
            }
            SDL_UpdateTexture(texture, nullptr, map.data, width * 4);

            gst_buffer_unmap(buffer, &map);
            gst_sample_unref(sample);

            // Render texture
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}