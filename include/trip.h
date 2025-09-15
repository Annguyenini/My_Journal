#pragma once
#ifndef TRIP_H
#define TRIP_H
#include <QObject>
#include <QWidget>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <iostream>
#include <json.hpp>
#include "configure.h"

class TripWorker : public QWidget{
    Q_OBJECT
    public:
    TripWorker();
    void processNewTrip(const std::string& tripName);
    unsigned int getTripId();
    void createDetail( unsigned int tripID,const std::string& tripName);
    void fileCheck();

    Q_SIGNAL void finishedSetupTrip();
};


class Trip : public QObject {
    Q_OBJECT

public:
    Trip(QVBoxLayout* mainlayout, QObject *parent=nullptr);
    
    // Main display controller
    void displayTrip();
    void setUpTrip();
    bool setupTripTable();
    bool setUpNewTrip();

private:
    QVBoxLayout* _mainlayout;
    QWidget* _mainWidget;
    // UI setup
    

    // Layout stack (switch between empty & active trip view)
    QStackedLayout* _tripLayoutStack;

    // Widgets
    QWidget* _activeTripWidget;
    QWidget* _emptyTripWidget;

    // Layouts
    QVBoxLayout* _activeTripLayout;
    QVBoxLayout* _emptyTripLayout;
    QHBoxLayout* _tripStatus;
    // QHBoxLayout* _tripInfobar;  // optional, kept for later
    QHBoxLayout* _tripSettingsBar;

    // Labels and inputs
    QLabel* _tripId;
    QLabel* _tripName;
    QLineEdit* _tripNameSetup;
    QPushButton* _submitBtn;
    QPushButton* _tripEditBoxButton;
    QPushButton* _tripStopTripButton;

    // Worker
    TripWorker* _tripWorkerObject;

    //
    bool isDisplay = false;
};




#endif // TRIP_H