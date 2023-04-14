#include "thread.h"

QStringList Thread::stripFiles(QString input)
{
    QStringList fileList;
    QStringList entries = input.split("\n");
    foreach(QString entry, entries) {
        int skip = 0;
        if (!entry.contains(".deb")) {skip = 1;}
        if (entry.contains("icons/compressed.gif")) {skip = 1;}
        if (skip == 0) {
            QString first = entry.mid(0, entry.indexOf(".deb"));
            first += ".deb";
            QString second = first.replace("<tr><td valign=\"top\"><img src=\"/icons/unknown.gif\" alt=\"[   ]\"></td><td><a href=\"", "");
            fileList << second;
        }
    }
    fileList.removeDuplicates();

    return fileList;
}
QStringList Thread::stripFolders(QString input)
{
    QStringList dirList;
    QStringList entries = input.split("\n");
//    qDebug() << entries.count();
    foreach(QString entry, entries) {
        int skip = 0;
        if (!entry.contains("DIR")) {skip = 1;}
        if (entry.contains("PARENTDIR")) {skip = 1;}
        if (skip == 0) {
            QString first = entry.mid(0, entry.indexOf("/\">"));
            QString second = first.replace("<tr><td valign=\"top\"><img src=\"/icons/folder.gif\" alt=\"[DIR]\"></td><td><a href=\"", "");
            dirList << second;
        }
    }
    dirList.removeDuplicates();

    return dirList;
}

QString Thread::getPercentage(QString input)
{
    input = input.replace("\r  ", "");
    input = input.replace("\r ", "");
    input = input.replace("\r", "");
    QString percentage = input.left(3);
    return percentage.simplified();
}

void Thread::run()
{ isRunning = 1; while(isRunning == 1) {
    if (function == "updateList") {
        emit func("updating");
        while(true) {
            qDebug() << "running";
            msleep(100);
            if (state == false) {break;}
        }
        QStringList files = stripFiles(output);
        QStringList folders = stripFolders(output);
        emit update(files, folders);
        emit func("updated");
    } else if (function == "dlOne") {
        emit func("download");
        while(true) {
            msleep(100);
            qDebug() << getPercentage(dlOutput) + "%";
            QStringList data;
            data << "Downloading " + dlInfo + "..." << getPercentage(dlOutput) << "Overall progress (0/1)" << getPercentage(dlOutput);
            updateProgress(data);
            if (state == false) {break;}
        }
        QStringList data; data << "Downloaded " + dlInfo << "100" << "Overall progress (1/1)" << "100";
        updateProgress(data);
    } else if (function == "dlList") {
        emit func("download");
        float quantity = files.count(); QString quantityStr = QString::number(int(quantity));
        int current = -1;
        float percentageAdd = 100 / quantity;
        foreach(QString file, files) {
            sendFile(file);
            current += 1;
            float modifier = percentageAdd * current;
            qDebug() << percentageAdd << "*" << current << "=" << percentageAdd * current;
            while(true) {
                msleep(100);
                QString percentage = getPercentage(dlOutput);
                int overallPercentage = percentage.toInt() / quantity + modifier;
                QStringList data; data << "Downloading " + file + "..." << percentage <<
                                          "Overall progress (" + QString::number(current) + "/" + quantityStr + ")" << QString::number(overallPercentage);
                updateProgress(data);
                if (state == false) {break;}
            }
        }
        QStringList data; data << "Downloaded" << "100" << "Overall progress (" + quantityStr + "/" + quantityStr + ")" << "100";
        updateProgress(data);
    }
    stopRunning();
}}

void Thread::stopRunning()
{
    isRunning = 0;
}
