#include "trip.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
using json = nlohmann::json;

TripWorker::TripWorker(){
    fileCheck();
};

void TripWorker::processNewTrip( const std::string& tripName){
    qDebug()<<"call process";
    qDebug()<<QString::fromStdString(tripName);
    
    qDebug()<<"pass";

    unsigned int id = getTripId();
    qDebug()<<"got id";
    createDetail(id,tripName);

}

void TripWorker::fileCheck(){
    json tripDetail;
    std::cout<<"checking file......\n";
    
    try{
        std::ifstream tripfile(Config::instance().tripDetailJson().string());
            tripDetail<<tripfile;

    }
    catch(const std::exception & e){
        std::cout<<"Fail to open detail file\n";
    }
    if(tripDetail["trip"].is_array() )return;
    else{
        std::ofstream out (Config::instance().tripDetailJson().string(),std::ios::trunc);
        tripDetail = json::object();
        tripDetail["trip"]=json::array();
        out<<tripDetail.dump(4);
        out.close();
        tripDetail.clear();
        out.close();
    }
}

unsigned int TripWorker::getTripId(){
    json tripDetail;
     unsigned int id = 0;
    if(!std::filesystem::exists(Config::instance().tripDetailJson().string())){
        std::cout<<"Missing trip detail file... Attempting to create!\n";
        try{
            std::ofstream tripFile(Config::instance().tripDetailJson().string());

        }
        catch(const std::exception &e){
            std::cout<<"Fail to create Trip detail file!: "<<e.what()<<std::endl;
        }

    }
    try{
        std::ifstream tripFile(Config::instance().tripDetailJson().string());
        tripDetail<<tripFile;

    }
    catch(const std::exception & e){
        std::cout<<"Fail to open Trip detail file!: "<<e.what()<<std::endl;

    }


    std::vector<unsigned int> existsTripId;
    if (tripDetail["trip"].empty()){
        return 1;
    }
    else{

    
        for(const auto& entry: tripDetail["trip"]){
            existsTripId.emplace_back(entry["id"]);
        }
        
        auto it = std::max_element(existsTripId.begin(),existsTripId.end());
        id =*it+1;

        return id;
    }
}   
void TripWorker::createDetail(unsigned int tripID, const std::string& tripName) {
    json tripDetail;
    auto path = Config::instance().tripDetailJson();
    std::ifstream in(path);
    if (in.is_open()){
        try{
            in >>tripDetail;
        }
        catch(...){
            tripDetail = json::object();
        }
        in.close();
    }

    qDebug() << "pass2-init";
    qDebug() << "trip type:" << QString::fromStdString(tripDetail["trip"].type_name());
    qDebug() << "Trips before push:" << tripDetail["trip"].size();

    tripDetail["trip"].push_back({
        {"id", tripID},
        {"trip_name", tripName},
        {"start_time", ""},
        {"end_time", ""},
        {"distance_km", 0},
        {"duration_minutes", 0}
    });
    qDebug() << "Trips after push:" << tripDetail["trip"].size();


    std::ofstream tripFile(path, std::ios::trunc);
    if (!tripFile.is_open()) {
        std::cerr << "Fail to write to trip detail file: " << path << std::endl;
        return;
    }

    tripFile << tripDetail.dump(4);
    tripFile.close();
    tripDetail.clear();
    qDebug() << "pass3-file";

    Config::instance().setTripId(tripID);
    Config::instance().setTripName(tripName);
    Config::instance().setTripActive(true);
    qDebug() << "pass4";
    qDebug() << "Trips after pdsdsdsdush:" << tripDetail["trip"].size();

    Q_EMIT finishedSetupTrip();
}
