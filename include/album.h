#ifndef ALBUM_H
#define ALBUM_H
#include <QObject>
#include <QLabel>
#include <QSlider>
#include <QPixmap>
#include <QPainter>
#include <QVBoxLayout>
#include <QDialog>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QWebEngineView>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include <QSettings>
#include <filesystem>
#include <sqlite3.h>
#include <gps.h>
#include "configure.h"
struct _albumsStruct {
        std::string path;
        std::string time;
        std::string type;     
};

class AlbumWorker : public QObject{
    Q_OBJECT

    private:
    
    std::vector<std::pair<std::string, _albumsStruct>> _albumCache;
  
    sqlite3* _db;
    GPSWorker _gpsObject;
    
   public:
   AlbumWorker();
   void initAlbumPath();
    void initDB();
    void insertToDB(const std::string& path,std::string type);
    void getAllDatafromAlbumDB(std::vector<std::pair<std::string, _albumsStruct>>& _albumCache);
    void syncData();
    void removeFromAlbum(const std::string& filename);


};
class Album : public QObject{
    Q_OBJECT

    private:
    AlbumWorker* _albumWorkerObject;
    std::vector<std::pair<std::string, _albumsStruct>> _albumCache;
    std::unordered_map<std::string, QLabel*> _albumLabelCache;
    QVBoxLayout* _mainlayout;
    QSettings * _settings = nullptr;

    sqlite3* _db;
    GPSWorker _gpsObject;
    QScrollArea* _scrollArea;
    QWidget* _albumWidget;
    QGridLayout* _albumLayout;
    bool _isDisplay =false;
    int GRIDROW=0;
    int GRIDCOLUMN = 0;
   public:
   Album(QVBoxLayout * layout, QObject *parent);
   void initAlbumPath();
    void initDB();
    void insertToDB(const std::string& path,std::string type);
    void getAllDatafromAlbumDB();
    bool setUpAlbumLabel();
    void showPreview(const std::string& filepath,const std::string& type);
    bool eventFilter(QObject* obj, QEvent* event)override;
    void closeAlbum();
    void loadToGrid();
    // bool loadToCache(std::string filename, QLabel* label);
    void adjustCache();
    void startAlbum();
    bool loadToCache(std::string filename, QLabel* label);
    bool displayAlbum();



};
#endif