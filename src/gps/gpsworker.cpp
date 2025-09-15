#include "gps.h"
#include <iostream>
#include <thread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <fstream>
#include <filesystem>
#include <sqlite3.h>
#include <ctime>
#include "json.hpp"
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
using Point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;

// Define your polygon type
using Polygon = boost::geometry::model::polygon<Point>;
using json = nlohmann::json;
namespace bg = boost::geometry;
std::unordered_map<std::string, GPSWorker::_gpsMetadataStruct> GPSWorker::_cache;
std::mutex GPSWorker::cacheMutex;
GPSWorker::GPSWorker(){
    // Delay all QObject creation to initialPort()
    // Paths will be set in initialPort()
}
void GPSWorker::initialPort(){
    qDebug() << "Current thread ID:" << QThread::currentThreadId();
    // Create QSettings and QSerialPort in the correct thread
    qDebug() << QString::fromStdString(Config::instance().databaseDir().string());
    qDebug() << QString::fromStdString(Config::instance().dbPath().string());
    qDebug() << QString::fromStdString(Config::instance().geoPolygonPath().string());
      
    GPSWorker::setUpDB();
    _serial = new QSerialPort(this);
    
    try{
        _serial->setPortName("/dev/serial0");
        _serial->setBaudRate(QSerialPort::Baud9600);
        _serial->setDataBits(QSerialPort::Data8);
        _serial->setParity(QSerialPort::NoParity);
        _serial->setStopBits(QSerialPort::OneStop);
        _serial->setFlowControl(QSerialPort::NoFlowControl);
    
    }
    catch(const std::exception &e ){
        qDebug()<<"Serial error:"<<e.what();
    }
    Q_EMIT void readyToShow();
    qDebug() << "readyToShow signal emitted";
}
bool GPSWorker::initPath(){
    if (!std::filesystem::exists(Config::instance().databaseDir().string())){
        try{
        qDebug()<<"Path doesnt exist..\n Attempt to create...\n";
        if(!std::filesystem::create_directories(Config::instance().databaseDir())){
            throw std::runtime_error("Fail to create database folder.\n");
        }
       }
       catch (std::exception &e){
        qDebug()<<"Database eror:" <<e.what();
        return false;
       }

    }
    if(!std::filesystem::exists(Config::instance().dbPath().string())){
        try{
            qDebug()<<"Db path doesn't exist..\nAttempt to create...\n";
            std::ofstream db (Config::instance().dbPath().string());
            if(!db){
                throw std::runtime_error("Fail to create gps db file.'n");
            }
            
            qDebug()<<"Done creating>>";
            db.close();
            
        }
        catch(const std::exception &e){
            qDebug()<<"Database erorr:"<<e.what();
            return false;
        }
    }
    if(!std::filesystem::exists(Config::instance().geoPolygonPath())){
        qDebug()<<"Geo file not found\n";
        return false;
    }
    return true;

}

void GPSWorker::setUpDB(){
    
    int rc = sqlite3_open(Config::instance().dbPath().string().c_str(),&_db);
    if (!_db) {
        qDebug() << "_db pointer is null!";
        return;
    }
    auto threadId = QThread::currentThreadId();
    qDebug() << "Current thread ID:" << threadId;
    qDebug() << "Is _db nullptr? " << (_db == nullptr);
    qDebug() << "sqlite3_open rc:" << rc;
    char* errMsg = nullptr;
    if (rc!= SQLITE_OK) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(_db) << std::endl;
    return;
    }
    // const char* createTableSQL="CREATE TABLE IF NOT EXISTS log (time TEXT, city TEXT, lat REAL, lng REAL )";
    const char* createTableSQL="CREATE TABLE IF NOT EXISTS log (time TEXT, tripId TEXT, tripName TEXT, city TEXT, lat REAL, lng REAL )";

    qDebug() << "SQL: " << createTableSQL;
    if (sqlite3_exec(_db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to enable WAL: " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else {
        std::cout << "WAL mode enabled!\n";
    }
    qDebug() << "sqlite3_db_errmsg: " << sqlite3_errmsg(_db);
    rc = sqlite3_exec(_db, createTableSQL,nullptr,nullptr,&errMsg);
    qDebug() << "sqlite3_exec rc:" << rc;
    if(rc != SQLITE_OK){
        qDebug()<<"SQL error (INSERT): "<<errMsg;
        sqlite3_free(errMsg);
    }
    qDebug()<<"pass2";
}

// std::string GPSWorker::getCurrentTime(){
//     auto now = std::chrono::system_clock::now();
//     std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
//     std::tm* now_tm = std::localtime(&now_time_t);

//     std::ostringstream oss;
//     oss << std::put_time(now_tm,"%Y-%m-%d_%H-%M-%S");
//     return oss.str();
// }


void GPSWorker::addingToCache(std::string time, const _gpsMetadataStruct & data){
    std::lock_guard<std::mutex> lock(cacheMutex);
    _cache[time] = data;
    if(_cache.size()>=10){
        sqlite3_exec(_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        // const char* sql ="INSERT INTO LOG (time,city, lat, lng) VALUES(?,?,?,?) ";
            const char* sql ="INSERT INTO LOG (time,city,tripId, tripName, lat, lng) VALUES(?,?,?,?,?,?) ";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(_db) << "\n";
            return;
        }
        for (const auto &[time,data]: _cache){
            sqlite3_bind_text (stmt,1,time.c_str(),-1,SQLITE_TRANSIENT);
            sqlite3_bind_text (stmt,2,data.city.c_str(),-1,SQLITE_TRANSIENT);
            sqlite3_bind_text (stmt,3, std::to_string(Config::instance().getTripId()).c_str(),-1,SQLITE_TRANSIENT);
            sqlite3_bind_text (stmt,4, Config::instance().getTripName().c_str(),-1,SQLITE_TRANSIENT );
            sqlite3_bind_double(stmt, 5, data.lat);
            sqlite3_bind_double(stmt, 6, data.lng);
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Insert failed: " << sqlite3_errmsg(_db) << "\n";
            }

            sqlite3_reset(stmt);             // Reuse statement
            sqlite3_clear_bindings(stmt);    // Clear previous bindings
        }

        sqlite3_finalize(stmt);
        sqlite3_exec(_db, "COMMIT;", nullptr, nullptr, nullptr);

        _cache.clear(); // Clear cache after flushing to DB
    }
    
}

void GPSWorker::loadGeoToCache(){
    if(_isGeoLoaded)return;
    std::vector<std::string>_cityvector;
    json vnGeo;
    std::ifstream geoOutput(Config::instance().geoPolygonPath().string());
    geoOutput>>vnGeo;
    try{
        for (const auto& feature :vnGeo["features"]){
            Polygon poly;
            if ((feature["geometry"].value("type",""))=="Polygon"){
                for(const auto& pair: feature["geometry"]["coordinates"][0]){
                    
                    float lng = pair[0];
                    float lat = pair[1];
                    bg::append(poly.outer(),Point(lng,lat));
                    
                }
            }
            else if((feature["geometry"].value("type",""))=="MultiPolygon"){
                for(const auto& rings: feature["geometry"]["coordinates"]){
                    for(const auto pairs : rings[0]){
                        float lng = pairs[0];
                        float lat = pairs[1];
                        bg::append(poly.outer(),Point(lng,lat));
                    }
                    
                }
            }
            cityNames CN{
                feature["properties"].value("name", "Unknown City"),
                feature["properties"].value("real_name","Unknown City")
            };
            _geoCache.push_back({CN,poly});
            _cityvector.push_back(CN.nameId);

            // qDebug() << "Loaded city:" << QString::fromStdString(CN.nameId);
        }
        if(_cityvector.size()==63){
            qDebug()<<"Loaded all";

        }
        else{
            qDebug()<<"Missing city";
        }
    }
    catch (const std::exception & e){
        qDebug()<<"Fail to load Geo Polygon data: "<<e.what();
    }
    _isGeoLoaded =true;
}

std::string GPSWorker::getCurrentCity(double lat, double lng){
    // qDebug()<<"pass3";
    if(!_isGeoLoaded)this->loadGeoToCache();
    // qDebug()<<"pass4";
    Point point (lng,lat);
    for(const auto& [names,poly]: _geoCache){
        if (bg::within(point,poly)|| bg::intersects(point, poly)){
            if(names.nameId != _currentCity && names.nameId != "Unknown City"){
                _currentCity = names.nameId;
                Q_EMIT cityChanged(names.nameId,names.realName.empty() ? "Unkown City" : names.realName);
                // if(!std::filesystem::exists(_imagesDir / _currentCity)){
                //     std::filesystem::create_directories(_imagesDir / _currentCity);
                // }
                return names.nameId;
            }
            
        }
    } 

    return "Unknown City";
}


void GPSWorker::startReadingFromGps(){
    qDebug()<<"pass33";
    try{
        if(!_serial->open(QIODevice::ReadOnly)){
            throw std::runtime_error("Failed to open serial port!");
        } // open to read form port
    }
    catch (const std::exception &e){
        qDebug()<<"Serial error"<<e.what();
    }
    
    //test case
    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, this ,[this](){

    
        _lat += 0.000124;
        _lng= 105.834322;
        this-> getCurrentCity(_lat,_lng);
        _gpsMetadataStruct data{
            _currentCity,
            _lat,
            _lng
        };
        this ->addingToCache(Config::instance().getCurrentTime(),data);
        Q_EMIT coordinatesUpdate (_lat== 0.0f ? 0.0 :_lat , _lng== 0.0f ? 0.0 : _lng);
        
    });
    timer ->start(1000);
    
    connect(_serial, &QSerialPort::readyRead,this ,[&](){
        if (!_serial) {
            qDebug() << "Serial port not initialized!";
            return;
        }
        // qDebug() << "_serial ptr:" << _serial << "thread:" << QThread::currentThread();

        _buffer.append(QString::fromUtf8(_serial->readAll()));
        int endIndex;
        
        // while ((endIndex = _buffer.indexOf("\r\n")) != -1){

        //     QString line = _buffer.left(endIndex);
        //     _buffer.remove(0,endIndex+2);
        //     // qDebug()<<line;
            
        //     QStringList parts = line.split(',');
            
        //     if (parts[0]=="$GPRMC" ){
        //         //_lat= parts[3].toFloat();
        //         //_lng= parts[5].toFloat();
        //         // testing
        //         _lat += 0.000124;
        //         _lng= 105.834322;
        //         this-> getCurrentCity(_lat,_lng);
        //         _gpsMetadataStruct data{
        //             _currentCity,
        //             _lat,
        //             _lng
        //         };
        //         this ->addingToCache(Config::instance().getCurrentTime(),data);
        //         Q_EMIT coordinatesUpdate (_lat== 0.0f ? 0.0 :_lat , _lng== 0.0f ? 0.0 : _lng);
        //     }
                
        //     else if (parts[0]=="$GPVTG"){

        //         float speed=parts[7].toFloat();
        //     }
        //     else{
        //         // qDebug()<<"Fail to read data or there is no good data!";
        //     }
        //     // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // }

        
        
        
    });
}


std::string GPSWorker:: returnCurrentCity(){
    qDebug()<<"returnCurrentCity called";
    qDebug()<<QString::fromStdString(_currentCity);
            return _currentCity;
        };
std::pair<float,float> GPSWorker:: getCoordinates(){
    qDebug()<<"return Coordinates";
    return {_lat,_lng};
}
GPSWorker::~GPSWorker(){
    
    if (_serial) {
        if (_serial->isOpen()) {
            _serial->close();
        }
        delete _serial;
        _serial = nullptr;
    }
}