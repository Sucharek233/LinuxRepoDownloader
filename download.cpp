#include "download.h"

QString output;

download::download()
{
    QObject::connect(&process, SIGNAL(started()), this, SLOT(start()));
    QObject::connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(stop(int, QProcess::ExitStatus)));
    QObject::connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(getOutput()));
    QObject::connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(getDlOutput()));
}

void download::start()
{
//    qDebug() << "start";
    emit updateState(true);
}

void download::stop(int, QProcess::ExitStatus)
{
    qDebug() << "stop";
    emit updateState(false);
    output.clear();
}

void download::getOutput()
{
    output += process.readAllStandardOutput();
    emit updateOutput(output);
}

void download::getDlOutput()
{
    QString dlOutput = process.readAllStandardError();
    emit updateDlOutput(dlOutput);
}

void download::getList()
{
    output = "";
    process.start("curl", QStringList() << url);
    process.waitForStarted();
}

void download::startDownload(QString file)
{
    output = "";
    process.start("curl", QStringList() << url + file << "--output" << path + "/" + file);
    qDebug() << "curl " + url + file + " --output " + path;
    process.waitForStarted();
}
