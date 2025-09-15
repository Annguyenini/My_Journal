#ifndef LOCATION_H
#define LOCATION_H
#include <QObject>
#include <QLabel>
#include <QVBoxLayout>
#include "libcam2opencv.h"
#include "mainWindow.h"
class Location{
    private:
        QLabel * _cityLabel;
        QLabel* _coorLabel;
    public:
        Location(QVBoxLayout* ml):mainlayout(ml){};
        ~Location(){};

        void cityLabelSetup();
        void coorLableSetup();

}



#endif