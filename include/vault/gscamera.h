#ifndef GSCAMERA_H
#define GSCAMERA_H

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QMutex>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <opencv2/opencv.hpp>

class GSCamera : public QWidget
{
    Q_OBJECT

public:
    explicit GSCamera(QWidget *parent = nullptr);
    ~GSCamera();

    bool start();
    void stop();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GstElement *pipeline;
    GstElement *appsink;

    QImage currentFrame;
    QMutex frameMutex;

    // Callback for appsink new-sample signal
    static GstFlowReturn onNewSample(GstAppSink *sink, gpointer user_data);
};

#endif // GSCAMERA_H
