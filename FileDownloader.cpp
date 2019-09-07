#include "FileDownloader.hpp"

#include <QJsonDocument>

FileDownloader::FileDownloader(QUrl imageUrl, QJsonObject json, QObject *parent) :
    QObject(parent)
{
    connect(
        &m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
        this, SLOT (fileDownloaded(QNetworkReply*))
        );

    QNetworkRequest request(imageUrl);
    request.setRawHeader("Content-Type", "application/json");
    m_WebCtrl.post(request, QJsonDocument(json).toJson());
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply* pReply) {
    m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const {
    return m_DownloadedData;
}
