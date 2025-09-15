#include "configure.h"
#include "trip.h"

Trip::Trip(QVBoxLayout* mainlayout, QObject *parent):_mainlayout(mainlayout),QObject(parent) {
}
void Trip::setUpTrip(){
    _mainWidget = new QWidget();
    _tripLayoutStack = new QStackedLayout(_mainWidget);

    _activeTripWidget = new QWidget();
    _emptyTripWidget  = new QWidget();

    _activeTripLayout = new QVBoxLayout();
    _emptyTripLayout  = new QVBoxLayout();
    _activeTripWidget ->setStyleSheet("background-color: #d3d3d3ff");
    _emptyTripWidget ->setStyleSheet("background-color: #d3d3d3ff");

    // Labels
    _tripId   = new QLabel();
    _tripName = new QLabel();

    // Layouts 
    _tripStatus      = new QHBoxLayout();
    // _tripInfobar     = new QHBoxLayout();
    _tripSettingsBar = new QHBoxLayout();

    // Name + button
    _tripNameSetup = new QLineEdit();
    _submitBtn     = new QPushButton("Submit");

    // Empty Trip Page
    _emptyTripLayout->addWidget(_tripNameSetup);
    _emptyTripLayout->addWidget(_submitBtn);
    _emptyTripWidget->setLayout(_emptyTripLayout);

    // Active Trip Page
    _activeTripLayout->addLayout(_tripStatus);
    // _activeTripLayout->addLayout(_tripInfobar);
    _activeTripLayout->addLayout(_tripSettingsBar);
    _activeTripWidget->setLayout(_activeTripLayout);
    _tripEditBoxButton   = new QPushButton("Edit");
    _tripStopTripButton  = new QPushButton("Stop");
    _tripSettingsBar->addWidget(_tripEditBoxButton);
    _tripSettingsBar->addWidget(_tripStopTripButton);

    // Add both to stack
    _tripLayoutStack->addWidget(_emptyTripWidget);
    _tripLayoutStack->addWidget(_activeTripWidget);
    _emptyTripWidget->hide();
    _activeTripWidget->hide();
    _mainlayout->insertWidget(4,_mainWidget);
    _tripLayoutStack->setCurrentWidget(0);
    // Trip Worker
    _tripWorkerObject = new TripWorker();
}

void Trip::displayTrip() {
    qDebug()<<"called";
    if(isDisplay){
        _emptyTripWidget->hide();
        _activeTripWidget->hide();
        isDisplay = false;
        
    }
    else{
        if (Config::instance().getTripActive()) {
            if(setupTripTable()){
                _tripLayoutStack->setCurrentWidget(_activeTripWidget);
                _activeTripWidget->show();
                _emptyTripWidget->hide();
            }
        else{
            std::cout<<"Fail to display trip table!\n";
        }
        } 
        else {
            
                if(setUpNewTrip()){
                    _tripLayoutStack->setCurrentWidget(_emptyTripWidget);
                    _emptyTripWidget->show();
                    _activeTripWidget->hide();

                }
            
            }
                isDisplay = true;

    }
    
    
}

bool Trip::setupTripTable() {
    qDebug()<<"called  trip";

    // Trip Id + Name
    
    _tripId->setText(QString::number(Config::instance().getTripId()));
    _tripName->setText(QString::fromStdString(Config::instance().getTripName()));

    _tripStatus->addWidget(_tripId);
    _tripStatus->addWidget(_tripName);

    // Trip Infobar (if you want these later)
    // _tripTotalPic   = new QLabel("Pic");
    // _tripTotalTime  = new QLabel("0 min");
    // _tripInfobar->addWidget(_tripTotalPic);
    // _tripInfobar->addWidget(_tripTotalTime);

    // Settings Bar
    
    // Connect buttons
    connect(_tripEditBoxButton, &QPushButton::clicked, this, [this]() {
        // TODO: edit trip
    });

    connect(_tripStopTripButton, &QPushButton::clicked, this, [this]() {
        // TODO: stop trip
        Config::instance().setTripId(0);
        Config::instance().setTripActive(false);
        Config::instance().setTripName("none");
        _activeTripWidget->hide();
        isDisplay = false;
    });
    return true;
}

bool Trip::setUpNewTrip() {
    qDebug()<<"called setuo new trip";

    _tripNameSetup->setPlaceholderText("Enter Trip Name"); // fixed typo: setPlaceholderTex -> setPlaceholderText
    qDebug()<<"pass1ewew3";
    qDebug()<<"pass13";
   qDebug()<<"pass1ew";
    // Hook submit to worker
    disconnect(_submitBtn, &QPushButton::clicked, nullptr, nullptr);

    connect(_submitBtn, &QPushButton::clicked, this, [this]() {
        qDebug()<<"pass1";
        std::string tripname= _tripNameSetup->text().toStdString();

        _tripWorkerObject->processNewTrip(tripname);
    });

    // Hook worker completion
    connect(_tripWorkerObject, &TripWorker::finishedSetupTrip, this, [this]() {
        _emptyTripWidget->hide();
        isDisplay = false;

    });
    return true;
}
