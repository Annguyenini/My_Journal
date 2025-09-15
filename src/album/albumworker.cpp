#include "album.h"
#include "gps.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
AlbumWorker::AlbumWorker(){
    initAlbumPath();
    initDB();
    syncData();
}

void AlbumWorker::initAlbumPath(){
}
void AlbumWorker::initDB(){
    int rc = sqlite3_open(Config::instance().albumDB().string().c_str(),&_db);
    
    char* errMsg = nullptr;
    if (rc != SQLITE_OK){
        qDebug()<<"Cant open db";
        return;
    }
    const char*  createTableSQL = "CREATE TABLE IF NOT EXISTS album (path TEXT , time TEXT, type TEXT)";
    rc = sqlite3_exec(_db, createTableSQL, nullptr,nullptr,&errMsg);

    if (rc!= SQLITE_OK){
        qDebug()<<"Fail to create the table";
    }
    if (sqlite3_exec(_db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to enable WAL: " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else {
        std::cout << "WAL mode enabled!\n";
    }


}
void AlbumWorker::insertToDB(const std::string& path,std::string type){
    std::string time = Config::instance().getCurrentTime();
    
    const char* insertToDB = "INSERT INTO Album (path,time,type) VALUES (?,?,?)";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(_db,insertToDB,-1,&stmt,nullptr)!= SQLITE_OK){
        qDebug()<<"Failed to prepare statement: " << sqlite3_errmsg(_db) << "\n";
        return;
    }
    sqlite3_bind_text (stmt, 1, path.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 2, time.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 3, type.c_str(),-1,SQLITE_TRANSIENT);
     if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(_db) << "\n";
    } else {
        std::cout << "Insert successful!\n";
    }

    sqlite3_finalize(stmt);
    // sqlite3_close(_db);

    return;
}
void AlbumWorker::getAllDatafromAlbumDB(std::vector<std::pair<std::string, _albumsStruct>>& _albumCache){
    qDebug() << "Fetching all data from album DB";
    const char* sql = "SELECT path,time,type FROM album ORDER BY time DESC";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Failed to prepare SELECT:" << sqlite3_errmsg(_db);
        return;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        std::string path = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        
        _albumsStruct at{
            path,
            time,
            type,
        };
        _albumCache.emplace_back(path, at);
    }
}


void AlbumWorker::syncData(){
    qDebug()<<"syning data";
    std::vector<std::pair<std::string, _albumsStruct>> _albumCache;
    this->getAllDatafromAlbumDB(_albumCache);
    std::vector <std::string> _foldercache;
    std::vector <std::string> different;
    for (const auto& entry : std::filesystem::directory_iterator(Config::instance().gallery())){
        if(std::filesystem::is_regular_file(entry.path())){
            _foldercache.emplace_back(entry.path().string());
            qDebug()<<QString::fromStdString(entry.path().string());
        }
    }
    for (auto vit = _albumCache.begin();vit!=_albumCache.end();++vit){
        std::string fn = vit->first;
        auto it = std::find(_foldercache.begin(),_foldercache.end(), fn);
        if (it == _foldercache.end()) 
        {
            qDebug()<<QString::fromStdString(fn);

            different.emplace_back(fn);
        }
        
    }
    
    for (const auto& entry : different) {
        std::string sql = "DELETE FROM album WHERE path = '" + entry + "';";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            qDebug() << "Failed to prepare DELETE:" << sqlite3_errmsg(_db);
            return;
        }
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        qDebug()<<"Delete from DB";
    }
    qDebug()<<"complete";

    
}

void AlbumWorker::removeFromAlbum(const std::string& filename){
    if(std::filesystem::remove(filename))qDebug()<<"Delete file successfully!";
    else{
        qDebug()<<"Fail to delete file!";
        return;
    }
    
    
    std::string sql = "DELETE FROM album WHERE path = '"+filename +"';";
    sqlite3_stmt* stmt = nullptr;
    if(sqlite3_prepare_v2(_db, sql.c_str(),-1,&stmt,nullptr)!= SQLITE_OK){
        qDebug()<<"fail to delete from db";
        return;
    }
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    qDebug()<<"Delete from DB";

    
    std::string mp4 =".mp4";
    std::string thmb ="_thumb.jpg";
    std::string filenamecp = filename;
    size_t pos = filename.find(mp4);
    if(pos != std::string::npos){
        filenamecp.erase(pos, mp4.length());
        filenamecp += thmb;
        std::string sql = "DELETE FROM album WHERE path = '"+filenamecp +"';";
        sqlite3_stmt* stmt = nullptr;
        if(sqlite3_prepare_v2(_db, sql.c_str(),-1,&stmt,nullptr)!= SQLITE_OK){
            qDebug()<<"fail to delete from db";
            return;
        }
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        qDebug()<<"Delete thumb from DB";
    }
    return;
}