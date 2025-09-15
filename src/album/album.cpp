#include "album.h"
#include <algorithm>


Album::Album(QVBoxLayout * layout, QObject *parent):_mainlayout(layout),QObject(parent){
        _albumWorkerObject =new AlbumWorker();
        this->setUpAlbumLabel();

};
bool Album::displayAlbum(){
    if(!_isDisplay){

        this->startAlbum();

        _albumWidget->setVisible(true);
        _isDisplay =true;
        qDebug()<<"Album Displayed";
        return true;
        
    }
    else if(_isDisplay){

        _albumWidget->setVisible(false);
        _isDisplay =false;
        qDebug()<<"Album hided";

        return false;
    }
}
void Album::startAlbum(){
    if(!_albumCache.empty()){
        _albumCache.clear();
    }
    _albumWorkerObject->getAllDatafromAlbumDB(_albumCache);
    qDebug() << "Album cache size:" << _albumCache.size();
    this->adjustCache();
    this->loadToGrid();
}
bool Album::loadToCache(std::string filename, QLabel* label){
    if (_albumLabelCache[filename] = label){
        return true;
    }
    else{
        return false;
    }

}

void Album::adjustCache(){
    if (_albumCache.empty()){
        return;
    }
    for( auto it = _albumLabelCache.begin();it!=_albumLabelCache.end();){
        auto fn1 = it->first;
        auto vit = std::find_if(_albumCache.begin(),_albumCache.end(),[&fn1](const auto& p ){
            return p.first == fn1;
        });
        if(vit == _albumCache.end()){
            QLabel* label = it->second;
            _albumLayout->removeWidget(label);
             delete label;
            it =_albumLabelCache.erase(it); //it will increase to the nest one;
            qDebug()<<"Delete from label cache";
        }
        else if(vit != _albumCache.end()){
            _albumCache.erase(remove_if(_albumCache.begin(),_albumCache.end(),[&fn1](const auto& p){
                 return p.first == fn1;
            }),_albumCache.end());
            it++;
            qDebug()<<"Delete from info cache";
        }
        qDebug()<<"Adjusting the cache";
    }
    qDebug()<<"Finish adjust the cache";
}
bool Album::setUpAlbumLabel(){
    
    qDebug()<<"runnnnnnnnnnnn";
    _scrollArea = new QScrollArea();
    _scrollArea ->setWidgetResizable(true);
    _albumWidget = new QWidget();
    _albumLayout = new QGridLayout(_albumWidget);
    _albumLayout -> setContentsMargins(0,0,0,0);
    _albumLayout -> setSpacing(5);
    _albumLayout -> setVerticalSpacing(5);
    _scrollArea->setWidget(_albumWidget);
    _albumWidget->setVisible(false);
    _mainlayout ->insertWidget(4,_scrollArea);

    return true;
}

void Album::loadToGrid(){
    int GRIDROW=0, GRIDCOLUMN = 0;
    for(const auto& pair : _albumCache){
        QString filename = QString::fromStdString(pair.first);
        if (filename.contains(".mp4"))continue;
        
        QLabel* label = new QLabel();
        qDebug()<<filename;
        QPixmap pixmap = QPixmap(filename).scaled(350, 350, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        if (filename.contains("_thumb")){
            filename.replace("_thumb.jpg", ".mp4");
            label->setProperty("filepath", filename);
            label->setProperty("type","video");
            QPixmap overlay = QPixmap(QString::fromStdString(Config::instance().playIconPath().string()));
            qDebug()<<QString::fromStdString(Config::instance().playIconPath().string());
            QPixmap result(pixmap.size());
            result.fill(Qt::transparent);

            QPainter painter(&result);
            painter.drawPixmap(0, 0, pixmap);

            // Ensure overlay has the same size or scale it
            QPixmap scaledOverlay = overlay.scaled(pixmap.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(0, 0, scaledOverlay);

            painter.end();

            label->setPixmap(result);
            label->setFixedSize(result.size());
            label->update(); 
        }
        else{
            label->setProperty("filepath", filename);
            label->setProperty("type","image");
            label->setPixmap(pixmap);
            label->setFixedSize(label->pixmap().size());
        }
        label->installEventFilter(this); 
        _albumLayout->addWidget(label,GRIDROW,GRIDCOLUMN);
        if (!this ->loadToCache(filename.toStdString(),label)){
            qDebug()<<"Fail to load to cache";
        }
    
        GRIDCOLUMN ++;
        if (GRIDCOLUMN>=2) {
            GRIDROW++;
            GRIDCOLUMN = 0;
        }
    }    
    qDebug()<<"Finished load to grid";
    qDebug()<<QString::number(_albumCache.size());
}


bool Album::eventFilter(QObject* obj, QEvent* event)  {
    if (event->type() == QEvent::MouseButtonPress) {
        if (QLabel* lbl = qobject_cast<QLabel*>(obj)) {
            qDebug() << "Label clicked!";
            QString filepath = lbl->property("filepath").toString();
            if(lbl->property("type")=="image"){
                showPreview(filepath.toStdString(),"image");
            }
            else if(lbl ->property("type") == "video"){
                showPreview(filepath.toStdString(),"video");
                qDebug()<<"pass1";
            }
            
            return true; // event handled
        }
    }
    return QObject::eventFilter(obj, event);
}
void Album::showPreview(const std::string& filepath,const std::string& type){
    QDialog* dlg = new QDialog();
    dlg->setWindowTitle(QString::fromStdString(filepath));
    QVBoxLayout* dlgLayout = new QVBoxLayout();
    dlg->setLayout(dlgLayout);

    if(type == "image"){

        QLabel* label = new QLabel();
        label->setPixmap(QPixmap(QString::fromStdString(filepath)).scaled(600,600,Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        dlgLayout->addWidget(label);
        
    }
    else if(type=="video"){
        // qDebug()<<"pass2";
        // QMediaPlayer* dlgplayer = new QMediaPlayer(dlg);
        // QVideoWidget* videoWidget =new QVideoWidget();
        // dlgplayer ->setVideoOutput(videoWidget);
        // dlgplayer ->setSource(QUrl::fromLocalFile(QString::fromStdString(filepath)));
        // dlgplayer->play(); // start playback
        // videoWidget ->setFixedSize(600,600);
        // connect(dlgplayer,&QMediaPlayer::mediaStatusChanged,[dlgplayer](QMediaPlayer::MediaStatus status){
        //     if (status == QMediaPlayer::EndOfMedia) {
        //         dlgplayer->setPosition(0); // rewind to start
        //         dlgplayer->play();          // replay
        //     }
        // });
        // dlgLayout->addWidget(videoWidget);
        QWebEngineView* webView = new QWebEngineView(dlg);
        // webView->setFixedSize(400, 400);
        webView->setGeometry(0,0,1000,1000);
        webView->load(QUrl::fromLocalFile(QString::fromStdString(filepath)));
        dlgLayout->addWidget(webView);
        
    }
    QPushButton* closeBtn = new QPushButton("Exit");
    
    connect (closeBtn, &QPushButton::clicked,dlg, &QDialog::accept);
    QPushButton* delBtn = new QPushButton("Delete");
    connect (delBtn, &QPushButton::clicked, this, [this,filepath](){
        _albumWorkerObject->removeFromAlbum(filepath);
    });
    
    dlgLayout->addWidget(closeBtn);
    dlgLayout->addWidget(delBtn);
    dlg->exec();
}

void Album::closeAlbum(){
    qDebug()<<"cleaning";
    _scrollArea->hide();
    _mainlayout->removeWidget(_scrollArea);
    delete _scrollArea;  // deletes child album widget and labels too
    _scrollArea = nullptr;
    _albumWidget = nullptr;
    _albumLayout = nullptr;
    _isDisplay =false;
    return;
}