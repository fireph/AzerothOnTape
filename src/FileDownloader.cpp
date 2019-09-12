#include "FileDownloader.hpp"

#include <QJsonDocument>

FileDownloader::FileDownloader(RequestType type, QUrl url, QJsonObject json, QObject *parent) :
        QObject(parent) {
    connect(
            &m_WebCtrl, SIGNAL (finished(QNetworkReply * )),
            this, SLOT (fileDownloaded(QNetworkReply * ))
    );

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    if (type == RequestType::POST) {
        m_WebCtrl.post(request, QJsonDocument(json).toJson());
    } else {
        m_WebCtrl.get(request);
    }
}

FileDownloader::~FileDownloader() {}

void FileDownloader::fileDownloaded(QNetworkReply *pReply) {
    m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const {
    return m_DownloadedData;
}
