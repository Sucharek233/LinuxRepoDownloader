#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileInfo>

class download : public QObject
{
    Q_OBJECT
public:
    explicit download(QObject *parent = 0);

    void setUrl(QString ur) {url = ur;}
    void setPath(QString pat) {path = pat;}
    void setFilename(QString name) {filename = name;}

    void setState(QString stat) {state = stat;}

signals:
    void updateList(QString);
    void updateState(bool);
    void updateInfo(QStringList);

    void sendError(QString);

public slots:
    void startDl();
    void onResult(QNetworkReply *reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void saveToFile();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QString url = "http://archive.ubuntu.com/ubuntu/pool/";
    QString path;
    QString filename;
    QString state;

    QString list;
};

#endif // DOWNLOADER_H
