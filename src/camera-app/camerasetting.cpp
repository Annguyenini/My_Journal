#include"camera.h"
#include "cameraworker.h"
void CameraWorker::setBrightness (float value){
    BRIGHTNESS = value;
    qDebug() << "Setting brightness to:" << BRIGHTNESS;
     for(auto &req : requests) {
        if (req && (req->status() == libcamera::Request::RequestPending || 
                       req->status() == libcamera::Request::RequestComplete)) {}
            camcontrols->set(libcamera::controls::Brightness, BRIGHTNESS);
            req->controls() = std::move(*camcontrols);
        }
    
    return;
};
void CameraWorker::setContrast (float value){
    CONTRAST = value;
    qDebug() << "Setting contrast to:" << CONTRAST;
    for(auto &req : requests) {
        if (req && (req->status() == libcamera::Request::RequestPending || 
                       req->status() == libcamera::Request::RequestComplete)) {}
            camcontrols->set(libcamera::controls::Contrast, CONTRAST);
            req->controls() = std::move(*camcontrols);
    }
    return;
};
void CameraWorker::setSaturation (float value){
    SATURATION = value;
    qDebug() << "Setting saturation to:" << SATURATION;
     for(auto &req : requests) {
        if (req && (req->status() == libcamera::Request::RequestPending || 
                       req->status() == libcamera::Request::RequestComplete)) {}
            camcontrols->set(libcamera::controls::Saturation, SATURATION);
            req->controls() = std::move(*camcontrols);
    }

    return;
};
void CameraWorker::setISO(float value){
    ISO = value;
    qDebug() << "Setting ISO to:" << ISO;
    this->applySettings();

    return;
};
void CameraWorker::setExposure(float value){
    EXPOSURE = value;
    qDebug() << "Setting exposure to:" << EXPOSURE;
    this->applySettings();

    return;
};
void CameraWorker::setRedGain(float rGain){
    
    R_GAIN = rGain;
    qDebug() << "Setting red gain to:" << R_GAIN;
    this->applySettings();
    return;
};    
void CameraWorker::setBlueGain(float bGain){
    
    B_GAIN = bGain;
    qDebug() << "Setting blue gain to:" << B_GAIN;
    this->applySettings();
    return;
};   

void CameraWorker::applySettings(){
    for(auto &req : requests) {
        if (req && (req->status() == libcamera::Request::RequestPending || 
                       req->status() == libcamera::Request::RequestComplete)) {
            
            // Directly modify existing controls - no std::move overhead
           
            camcontrols->set(libcamera::controls::Brightness, BRIGHTNESS);
            camcontrols->set (libcamera::controls::Contrast,CONTRAST);
            camcontrols->set(libcamera::controls::Saturation, SATURATION);
            // // camcontrols->set(libcamera::controls::AnalogueGain, ISO );
            // // camcontrols->set (libcamera::controls::ExposureTime,EXPOSURE);
            // camcontrols->set(libcamera::controls::ColourGains, { R_GAIN, B_GAIN });
            qDebug() << "Applying camera settings:";
            req->controls() = std::move(*camcontrols);
        }
    }
    

}

void Camera::cameraSettingsLayout(){
    _settingsbarLayout = new QHBoxLayout();
    _brightnessWidget = new QWidget();
    _contrastWidget = new QWidget();
    _saturationWidget = new QWidget();


    this->setUpBrightnessButtons();
    this->setUpContrastButtons();
    this->setUpSaturationButtons();
    _settingsbarLayout ->addWidget(_brightnessWidget);
    _settingsbarLayout ->addWidget(_contrastWidget);
    _settingsbarLayout ->addWidget(_saturationWidget);

       
}
void Camera::displayCameraSettingbar(){
    if(!_isSettingBarDisplay){
        _brightnessWidget->setVisible(true);
        _contrastWidget->setVisible(true);
        _saturationWidget->setVisible(true);
        _isSettingBarDisplay = true;
        _mainlayout->insertLayout(1,_settingsbarLayout);
        qDebug()<<"show";


    }
    else{
        _mainlayout->removeItem(_settingsbarLayout);
        _brightnessWidget->setVisible(false);
        _contrastWidget->setVisible(false);
        _saturationWidget->setVisible(false);
        _isSettingBarDisplay = false;
        qDebug()<<"hide";
    }
}
void Camera::setUpBrightnessButtons(){
    _brightnessLayout = new QVBoxLayout(_brightnessWidget);
    _brightnessUpButton = new QPushButton("Brightness Up");
    _brightnessDownButton = new QPushButton("Brightness Down");
    _brightnessUpButton->setStyleSheet(_btn_setting_properties);
    _brightnessDownButton->setStyleSheet(_btn_setting_properties);
    _brightnessLabel =new QLabel();
    _brightnessLabel ->setText(QString("Brightness: ")+QString::number(BRIGHTNESSValue *100.0f));
    _brightnessLabel->setStyleSheet("color: white; font-size: 15px");

    connect(_brightnessUpButton, &QPushButton::clicked, this, [this](){
        BRIGHTNESSValue += 0.02f;
         if (BRIGHTNESSValue > 0.5f) {
            BRIGHTNESSValue = 0.5f; // Clamp to max value
        }
        _camera->setBrightness(BRIGHTNESSValue);
        _brightnessLabel ->setText(QString("Brightness: ")+QString::number(BRIGHTNESSValue *100.0f));

    });
    connect(_brightnessDownButton, &QPushButton::clicked, this, [this](){
        BRIGHTNESSValue -= 0.02f;
        if (BRIGHTNESSValue < -0.5f) {
            BRIGHTNESSValue = -0.5f; // Clamp to max value
        }
        _camera->setBrightness(BRIGHTNESSValue);
        _brightnessLabel ->setText(QString("Brightness: ")+QString::number(BRIGHTNESSValue *100.0f));

    });
    _brightnessLayout->insertWidget(0,_brightnessUpButton);
    _brightnessLayout->insertWidget(1,_brightnessLabel);
    _brightnessLayout->insertWidget(2,_brightnessDownButton);
    

}
void Camera::setUpContrastButtons() {
    _contrastLayout = new QVBoxLayout(_contrastWidget);

    _contrastUpButton = new QPushButton("Contrast Up");
    _contrastDownButton = new QPushButton("Contrast Down");
    _contrastUpButton->setStyleSheet(_btn_setting_properties);
    _contrastDownButton->setStyleSheet(_btn_setting_properties);

    _contrastLabel = new QLabel();
    _contrastLabel->setText("Contrast: " + QString::number(CONTRASTValue));
    _contrastLabel->setStyleSheet("color: white; font-size: 15px");

    // Button connections
    connect(_contrastUpButton, &QPushButton::clicked, this, [this]() {
        CONTRASTValue += 0.02f;
        if (CONTRASTValue > 2.0f) CONTRASTValue = 2.0f; // max clamp
        _camera->setContrast(CONTRASTValue);
        _contrastLabel->setText("Contrast: " + QString::number(CONTRASTValue));
    });

    connect(_contrastDownButton, &QPushButton::clicked, this, [this]() {
        CONTRASTValue -= 0.02f;
        if (CONTRASTValue < -1.0f) CONTRASTValue = -1.0f; // min clamp
        _camera->setContrast(CONTRASTValue);
        _contrastLabel->setText("Contrast: " + QString::number(CONTRASTValue));
    });

    _contrastLayout->addWidget(_contrastUpButton);
    _contrastLayout->addWidget(_contrastLabel);
    _contrastLayout->addWidget(_contrastDownButton);

}

void Camera::setUpSaturationButtons() {
    _saturationLayout = new QVBoxLayout(_saturationWidget);

    _saturationUpButton = new QPushButton("Saturation Up");
    _saturationDownButton = new QPushButton("Saturation Down");
    _saturationUpButton->setStyleSheet(_btn_setting_properties);
    _saturationDownButton->setStyleSheet(_btn_setting_properties);

    _saturationLabel = new QLabel();
    _saturationLabel->setText("Saturation: " + QString::number(SATURATIONValue));
    _saturationLabel->setStyleSheet("color: white; font-size: 15px");

    connect(_saturationUpButton, &QPushButton::clicked, this, [this]() {
        SATURATIONValue += 0.05f;
        if (SATURATIONValue > 2.5f) SATURATIONValue = 4.0f;
        _camera->setSaturation(SATURATIONValue);
        _saturationLabel->setText("Saturation: " + QString::number(SATURATIONValue));
    });

    connect(_saturationDownButton, &QPushButton::clicked, this, [this]() {
        SATURATIONValue -= 0.05f;
        if (SATURATIONValue < -1.0f) SATURATIONValue = -2.0f;
        _camera->setSaturation(SATURATIONValue);
        _saturationLabel->setText("Saturation: " + QString::number(SATURATIONValue));
    });

    _saturationLayout->addWidget(_saturationUpButton);
    _saturationLayout->addWidget(_saturationLabel);
    _saturationLayout->addWidget(_saturationDownButton);

}

