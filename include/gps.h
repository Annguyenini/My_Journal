#ifndef GPS_H
#define GPS_H

#include <QSerialPort>
#include <QObject>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QThread>
#include <QSettings>
#include <QSerialPortInfo>
#include <filesystem>
#include <QSettings>
#include <sqlite3.h>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <utility>
// #include "mainwindow.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include "configure.h"
using Point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using Polygon = boost::geometry::model::polygon<Point>;
class GPSWorker: public QObject{
    Q_OBJECT

    private:
        struct _gpsMetadataStruct{
            std::string city;
            float lat;
            float lng;
        };

        struct cityNames{
            std::string nameId;
            std::string realName;
        };    

        inline static std::string _currentCity = "";
        inline static std::string _currentRealnamecity = "";
        static std::unordered_map<std::string, _gpsMetadataStruct> _cache;
        static std::mutex cacheMutex;
        float _speed=0;
        inline static float _lat = 21.02f;
        inline static float _lng = 0.0f;
        bool initPath();
        bool _isGeoLoaded = false;
        std::vector<std::pair<cityNames,Polygon>> _geoCache;
        QSerialPort* _serial = nullptr;
        sqlite3* _db;
        QString _buffer;


        
    public:
        GPSWorker();
        ~GPSWorker();
        void initialPort();
        void startReadingFromGps();
        void setUpDB();
        void addingToCache(std::string time, const _gpsMetadataStruct & data);
        void startThread();
        // std::string getCurrentTime();
        std::string getCurrentCity(double lat, double lng);        
        
        Q_SIGNAL void coordinatesUpdate(const float& lat, const float& lng);
        Q_SIGNAL void cityChanged(std::string nameId, std::string realName);
        Q_SIGNAL void readyToShow();
        void loadGeoToCache();
        // std::filesystem::path getCityImageDir();
        std::string returnCurrentCity();
        std::pair<float,float> getCoordinates();
};
class GPS: public QObject{
    Q_OBJECT
    private:
        QLabel*  _cityLabel;
        QLabel* _coordinatesLabel; 
        QVBoxLayout* _mainlayout;
        QThread* _workerThread;
        GPSWorker* _worker;

    public:
    explicit GPS(QVBoxLayout * layout, QObject *parent = nullptr);
    void setUpGps();
    ~GPS();
    GPSWorker* worker() const { return _worker; }
    void handleCityUpdate(const std::string &nameId, const std::string &realName);
    void handleCoordinatesUpdate(float lat, float lng);
    void stop();
    void hideLabel();
    void showLabel();
    Q_SIGNAL void stopWorker();

    
};

#endif