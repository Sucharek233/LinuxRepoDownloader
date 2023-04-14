#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class download : public QObject
{
    Q_OBJECT
public:
    download();

    void getList();

    void setUrl(QString ur) {url = ur;}
    void setPath(QString pat) {path = pat;}

    void stop() {process.terminate();}

public slots:
    void start();

    void stop(int, QProcess::ExitStatus);

    void getOutput();
    void getDlOutput();

    void startDownload(QString file);

signals:
    void updateOutput(QString);
    void updateDlOutput(QString);
    void updateState(bool);

private:
    QProcess process;

    QString url = "http://archive.ubuntu.com/ubuntu/pool/";
    QString path;
};

#endif // DOWNLOAD_H
