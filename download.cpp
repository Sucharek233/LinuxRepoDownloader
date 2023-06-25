#include "download.h"

QElapsedTimer timer;

download::download(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &download::onResult);

    timer.restart();
}

void download::startDl()
{
    emit updateState(true);

    QFile check(path + filename);
    if (check.exists()) {
        check.remove();
    }

    QUrl link(url);
    QNetworkRequest request;
    request.setUrl(link);
    qDebug() << "Download...";
    reply = manager->get(request);
    reply->setReadBufferSize(1000);
    connect(reply, &QNetworkReply::downloadProgress, this, &download::onDownloadProgress);
    connect(reply, &QNetworkReply::readyRead, this, &download::saveToFile);
}

qint64 previous;
double dlSpeed;
void download::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (timer.elapsed() == 0) {timer.start();}
    if (timer.elapsed() >= 1000) {
        double change = bytesReceived - previous;
        dlSpeed = change / 1000;
        previous = bytesReceived;
        timer.restart();
    }
    double percentage = static_cast<qreal>(bytesReceived) / bytesTotal * 100.0;

    QString details = QString::number(bytesReceived / 1000) + " KB/" + QString::number(bytesTotal / 1000) + " KB "
                      "(" + QString::number(dlSpeed) + " KB/s) " + QString::number(percentage) + "%";
    qDebug() << "Download progress:" << details;
    qDebug() << "Download speed:" << dlSpeed << "KB/S";


    QStringList info;
    info << QString::number(percentage) << QString::number(bytesReceived) << QString::number(bytesTotal) << QString::number(dlSpeed);
    emit updateInfo(info);
}

void download::saveToFile()
{
    if (state == "list") {
        list += reply->readAll();
    } else {
        QFile *file = new QFile(path + "pending." + filename);
        file->open(QFile::WriteOnly | QFile::Append);
        file->write(reply->readAll());
        file->close();
    }
}

void download::onResult(QNetworkReply *reply)
{
    if (reply->error()) {
        //NOT FINISHED!!!
        qDebug() << "ERROR";
        qDebug() << reply->errorString();
        emit sendError(reply->errorString());
    } else {
        if (state == "list") {
            list += reply->readAll();
            emit updateList(list);
            list.clear();
        } else {
            QFile *file = new QFile(path + "pending." + filename);
            if (file->open(QFile::WriteOnly | QFile::Append)) {
                file->write(reply->readAll());
                file->close();

//                WHY TF DOES THIS EXIST??????
//                QFile check(path + "pending." + filename);
//                if (check.exists()) {check.remove();}
//                WHY TF DOES THIS EXIST??????

                file->rename(path + filename);
                qDebug() << "final save to" << path + filename;

                qDebug() << "Download complete";
                reply->deleteLater();
            }
        }
    }

    emit updateState(false);
}
