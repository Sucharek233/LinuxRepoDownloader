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
            qDebug() << first;
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

QString Thread::getSizes()
{
    QString currSize = QString::number(int(info.at(1).toInt() / 1000));
    QString totalSize = QString::number(int(info.at(2).toInt() / 1000));
    QString size = " (" + currSize + "KB/" + totalSize + "KB)";
    return size;
}

void Thread::run()
{
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
            QString percentage = info.at(0);
            QStringList data;
            data << "Downloading " + fileName + getSizes() + "..." << percentage << "Overall progress (0/1)" << percentage;
            updateProgress(data);
            if (state == false) {break;}
        }
        QStringList data; data << "Downloaded " + fileName + getSizes() << "100" << "Overall progress (1/1)" << "100";
        updateProgress(data);
        if (!err.isEmpty()) {
            QStringList fail;
            fail << "Error while downloading these following packages:\n" + fileName <<
                    "Detailed info\n\n" + fileName + ":\n" + err;
            showError(fail);
        }
    } else if (function == "dlList") {
        emit func("download");
        float quantity = files.count(); QString quantityStr = QString::number(int(quantity));
        int current = -1;
        float percentageAdd = 100 / quantity;
        double totalKB = 0;

        err.clear();
        QStringList packages;
        QStringList errors;
        foreach(QString file, files) {
            sendFile(file);
            current += 1;
            float modifier = percentageAdd * current;
            qDebug() << percentageAdd << "*" << current << "=" << percentageAdd * current;
            while(true) {
                msleep(100);
                QString percentage = info.at(0);
                int overallPercentage = percentage.toInt() / quantity + modifier;
                QStringList data; data << "Downloading " + file + getSizes() + "..." << percentage <<
                                          "Overall progress (" + QString::number(current) + "/" + quantityStr + ") (Total downloaded: " +
                                          QString::number(int(totalKB)) + " KB)" << QString::number(overallPercentage);
                updateProgress(data);
                if (state == false) {break;}
            }
            totalKB += info.at(2).toInt() / 1000;

            if (!err.isEmpty()) {
                packages << file;
                errors << err;
            }
            err.clear();
        }
        if (!packages.isEmpty() && !errors.isEmpty()) {
            QString header = "Error while downloading these following packages:";
            QString details = "Detailed info:";
            int i = 0;
            foreach(QString package, packages) {
                header += "\n" + package;
                QString currErr = errors.at(i);
                details += "\n\n" + package + ":\n" + currErr;
                i += 1;
            }
            QStringList final;
            final << header << details;
            showError(final);
        }

        QStringList data; data << "Downloaded" << "100" << "Overall progress (" + quantityStr + "/" + quantityStr + ") (Total downloaded: " +
                                  QString::number(int(totalKB)) + " KB)" << "100";
        updateProgress(data);
    }
}
