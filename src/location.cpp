#include "location.h"


void Location::cityLabelSetup(){
    _cityLabel=new QLabel();
    _cityLabel->setObjectName("City-label");
    _cityLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
    mainlayout->addWidget(_cityLabel)

}
void location::updateLocation(){
    
}
void Location::coorLableSetup(){
    _coorLabel = new QLabel();
    _coorLabel->setObjectName("Coor-label");
    _coorLabel->setStyleSheet("color: white; font-size: 30px; margin: 0; padding: 0;");
    mainlayout->addWidget(_coorLabel);
}
Location::~location(){
    delete _citylabel;
    delete _coorLabel;
}