#pragma once
#include <filesystem>
#include <QSettings>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>

class Config {
public:
    // 🔑 Singleton accessor
    static Config& instance() {
        static Config inst;  // created once, reused everywhere
        return inst;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    QString btnProperties() const { return _btn_properties; }
    QString actionPro() const { return _actionPro; }

    const std::filesystem::path& databaseDir() const { return _databaseDir; }
    const std::filesystem::path& dbPath() const { return _dbPath; }
    const std::filesystem::path& geoPolygonPath() const { return _geoPolygonPath; }
    const std::filesystem::path& gallery() const { return _gallery; }
    const std::filesystem::path& albumDB() const { return _albumDB; }
    const std::filesystem::path& playIconPath() const { return _playIconPath; }
    const std::filesystem::path& mainBackgroundPath() const { return _mainbackgroundPath; }
    const std::filesystem::path& tripDetailJson() const {return _tripDetailJson;}
   
    void setTripActive(bool active) {
        TRIP_ACTIVE = active;
        _settings->setValue("Settings/tripActive", TRIP_ACTIVE ? 1 : 0);
    }
   
    void setTripId(unsigned int id) {
        TRIP_ID = id;
        _settings->setValue("Settings/tripID", TRIP_ID);

    }
    void setTripName(std::string name) {
        TRIP_NAME = name;
        _settings->setValue("Settings/tripID", TRIP_ID);

    }
    unsigned int getTripId() const { return TRIP_ID; }
    bool getTripActive() const { return TRIP_ACTIVE; }
    std::string getTripName() const {return TRIP_NAME;}

    
    std::string getCurrentTime() const {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time_t);

        std::ostringstream oss;
        oss << std::put_time(now_tm, "%Y-%m-%d_%H-%M-%S");
        return oss.str();
    }

private:
    Config() {
        _parentDir = std::filesystem::current_path().parent_path();
        _settings = std::make_unique<QSettings>(
            QString::fromStdString((_parentDir / "Configure.ini").string()),
            QSettings::IniFormat
        );

        _databaseDir = _parentDir / _settings->value("Database/path").toString().toStdString();
        _dbPath = _parentDir / _settings->value("Database/gpsPath").toString().toStdString();
        _geoPolygonPath = _parentDir / _settings->value("Database/geoPolygonPath").toString().toStdString();
        _gallery = _parentDir / _settings->value("Database/galleryPath").toString().toStdString();
        _albumDB = _parentDir / _settings->value("Database/albumsDB").toString().toStdString();
        _playIconPath = _parentDir / _settings->value("Assets/playicon").toString().toStdString();
        _mainbackgroundPath = _parentDir / _settings->value("Assets/mainbackground").toString().toStdString();
        _tripDetailJson = _parentDir / _settings->value("Database/tripDetail").toString().toStdString();
        QString var = _settings->value("Settings/tripActive").toString();
        TRIP_ID = _settings->value("Settings/tripID").toInt();
        TRIP_ACTIVE = var.toInt() != 0;
        TRIP_NAME = _settings->value("Settings/tripName").toString().toStdString();

        _btn_properties = R"(QPushButton {
            background-color: #000000;
            color: white;
            font-size: 20px;
            padding: 10px 20px;
            border: none;
            border-radius: 8px;
        }
        QPushButton:hover {
            background-color: #005f99;
        })";

        _actionPro = R"(QMenu {
          background-color: #222;
          color: #fff;
          border: 1px solid #555;
        }
        QMenu::item {
          padding: 10px 70px;
          min-height: 40px;
          font-size: 16px;
        }
        QMenu::item:selected {
          background-color: #555;
        })";
    }

    QString _btn_properties;
    QString _actionPro;
    std::filesystem::path _parentDir;
    std::unique_ptr<QSettings> _settings;

    std::filesystem::path _databaseDir;
    std::filesystem::path _dbPath;
    std::filesystem::path _geoPolygonPath;
    std::filesystem::path _gallery;
    std::filesystem::path _albumDB;
    std::filesystem::path _playIconPath;
    std::filesystem::path _mainbackgroundPath;
    std::filesystem::path _tripDetailJson;
    unsigned int TRIP_ID;
    bool TRIP_ACTIVE;
    std::string TRIP_NAME;
};

// class Config {
// public:
//     static Config& instance() {
//         static Config cfg;
//         return cfg;
//     }

//     QSettings* settings;
//     std::filesystem::path parentDir;
//     unsigned int tripID;
//     bool tripActive;

// private:
//     Config() {
//         parentDir = std::filesystem::current_path().parent_path();
//         settings = new QSettings(QString::fromStdString((parentDir / "Configure.ini").string()), QSettings::IniFormat);
//         tripID = settings->value("Settings/tripID", 0).toUInt();
//         tripActive = settings->value("Settings/tripActive", 0).toInt() != 0;
//     }
// };
