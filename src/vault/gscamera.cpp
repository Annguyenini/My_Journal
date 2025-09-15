#include "gscamera.h"
#include <QDebug>
#include <QPainter>
#include <QMutexLocker>

GSCamera::GSCamera(QWidget *parent) : QWidget(parent) {
    gst_init(nullptr, nullptr);
    pipeline = nullptr;
    appsink = nullptr;
}

GSCamera::~GSCamera() {
    stop();
    gst_deinit();
}

bool GSCamera::start() {
    const char* pipeline_desc =
        "libcamerasrc ! video/x-raw,width=1920,height=1080,framerate=30/1 ! "
        "videoconvert ! video/x-raw,format=BGR ! appsink name=mysink drop=true sync=false max-buffers=1";

    GError *err = nullptr;
    pipeline = gst_parse_launch(pipeline_desc, &err);
    if (!pipeline) {
        qWarning() << "Failed to create pipeline:" << (err ? err->message : "unknown");
        if (err) g_error_free(err);
        return false;
    }

    appsink = gst_bin_get_by_name(GST_BIN(pipeline), "mysink");
    if (!appsink) {
        qWarning() << "Failed to get appsink";
        gst_object_unref(pipeline);
        pipeline = nullptr;
        return false;
    }

    // Configure appsink to call our callback
    gst_app_sink_set_emit_signals(GST_APP_SINK(appsink), true);
    g_signal_connect(appsink, "new-sample", G_CALLBACK(&GSCamera::onNewSample), this);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    qDebug() << "Camera started";
    return true;
}

void GSCamera::stop() {
    if (!pipeline) return;

    gst_element_set_state(pipeline, GST_STATE_NULL);
    if (appsink) g_object_unref(appsink);
    gst_object_unref(pipeline);

    pipeline = nullptr;
    appsink = nullptr;
}

GstFlowReturn GSCamera::onNewSample(GstAppSink *sink, gpointer user_data) {
    GSCamera *self = static_cast<GSCamera*>(user_data);
    GstSample *sample = gst_app_sink_pull_sample(sink);
    if (!sample) return GST_FLOW_OK;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        cv::Mat frame(1080, 1920, CV_8UC3, (uchar*)map.data);

        QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
        {
            QMutexLocker locker(&self->frameMutex);
            self->currentFrame = img.copy(); // detach memory safely
        }

        self->update(); // trigger paintEvent
        gst_buffer_unmap(buffer, &map);
    }

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

void GSCamera::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QMutexLocker locker(&frameMutex);
    if (!currentFrame.isNull()) {
        painter.drawImage(rect(), currentFrame);
    }
}
