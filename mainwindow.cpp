#include "mainwindow.h"
#include "ui_mainwindow.h"

QString stylesheet;

QStringList currFiles;

QString configPath;

bool latestOpt;
bool debugOpt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&thread,SIGNAL(func(const QString&)),SLOT(function(const QString&)), Qt::AutoConnection);
    QObject::connect(&thread,SIGNAL(update(const QStringList&, const QStringList&)),SLOT(updateList(const QStringList&, const QStringList&)), Qt::AutoConnection);

    QObject::connect(&dl,SIGNAL(updateOutput(const QString&)),SLOT(sendOutput(const QString&)), Qt::AutoConnection);
    QObject::connect(&dl,SIGNAL(updateDlOutput(const QString&)),SLOT(sendDlOutput(const QString&)), Qt::AutoConnection);
    QObject::connect(&dl,SIGNAL(updateState(const bool&)),SLOT(sendState(const bool&)), Qt::AutoConnection);

    QObject::connect(&thread,SIGNAL(updateProgress(const QStringList&)),&prog,SLOT(unpack(const QStringList&)), Qt::AutoConnection);

    QObject::connect(&thread,SIGNAL(sendFile(const QString&)),&dl,SLOT(startDownload(const QString&)), Qt::AutoConnection);

    QObject::connect(&prog,SIGNAL(stopDl()),SLOT(stopDl()), Qt::AutoConnection);

    initInfo();
    update(url);

    stylesheet = "QMessageBox QLabel {font-size: 18px;} QMessageBox QPushButton {font-size: 14px;}";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(QString url)
{
    dl.setUrl(url);

    thread.setFunction("updateList");
    thread.start();
    dl.getList();
}

void MainWindow::initInfo()
{
    configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    QDir checkFolder(configPath); if (!checkFolder.exists()) {checkFolder.mkdir(configPath);}
    QFile config(configPath + "config.ini");
    if (!config.exists()) {
        config.open(QIODevice::ReadWrite);
        config.write("\n"
                     "http://archive.ubuntu.com/ubuntu/pool/\n"
                     "0\n"
                     "0");
        config.close();
    }

    config.open(QIODevice::ReadOnly);
    path = config.readLine(); path = path.replace("\n", "");

    QString repo = config.readLine(); repo = repo.replace("\n", "");
    latestOpt = config.readLine().toInt();
    debugOpt = config.readLine().toInt();
    if (repo == "\n" || repo.isEmpty()) {repo = url;}
    opts.set(QStringList() << repo << QString::number(latestOpt) << QString::number(debugOpt));

    config.close();

    dl.setPath(path);
    url = repo;
    dl.setUrl(repo);
}
void MainWindow::setPath(QString newPath)
{
    QFile config(configPath + "config.ini");
    config.open(QIODevice::ReadWrite);

    QString getOpts = config.readAll();
    QStringList one = getOpts.split("\n");
    QString toWrite = newPath + "/\n" + one.at(1) + "\n" + one.at(2) + "\n" + one.at(3);

    config.resize(0);
    config.write(toWrite.toUtf8());
    config.close();

    initInfo();
}
void MainWindow::setOptions(QStringList options)
{
    QFile config(configPath + "config.ini");
    config.open(QIODevice::ReadWrite);
    QString keepPath = path + "\n";
    if (path.isEmpty()) {keepPath = "\n";}
    QString toWrite = keepPath + options.join("\n");
    config.resize(0);
    config.write(toWrite.toUtf8());
    config.close();

    initInfo();
}

void MainWindow::function(QString func)
{
    if (func == "updating") {
        ui->pushButton_Sort->setEnabled(false);
        ui->listWidget_List->setEnabled(false);
        ui->statusbar->showMessage("Updating list...");
    } else if (func == "updated") {
        ui->pushButton_Sort->setEnabled(true);
        ui->listWidget_List->setEnabled(true);
        ui->listWidget_List->scrollToTop();
        ui->statusbar->showMessage("List updated", 3000);
        ui->statusbar->showMessage("Current directory: " + url);
    } else if (func == "download") {
        ui->pushButton_Progress->setEnabled(true);
    }
}

void MainWindow::updateList(QStringList files, QStringList folders)
{
    ui->listWidget_List->clear();
    currFiles.clear();

    QListWidgetItem* up = new QListWidgetItem();
    QIcon icon; icon = QIcon(":/icons/folder.png");

    up->setIcon(icon);
    up->setText("Parent directory");
    ui->listWidget_List->addItem(up);

    QString saveName;
    QString saveVer;
    QString stuff;
    QString latest;

    QStringList names;
    QStringList versions;
    foreach (QString entry, files) {
        QListWidgetItem* item = new QListWidgetItem();
        QIcon icon; icon = QIcon(":/icons/file.png");

        QString saveEntry = entry;
        QString name = entry.mid(0, entry.indexOf("_"));
        QString renName = entry.replace(name , "");
        QString renUnderscore = renName.remove(0, 1);
        QString version = renUnderscore.mid(0, renUnderscore.indexOf("_"));
        version = version.replace(".deb", "");
//        qDebug() << "\n";
//        qDebug() << name;
//        qDebug() << version;

        if (saveName != name) {
            stuff += "\n" + name + "\t";

            if (!saveName.isEmpty()) {
                latest += "latest " + saveName + " = " + saveVer + "\n";
                names << saveName; versions << saveVer;
            }
        }
        if (saveVer != version) {
            stuff += version + "\t";
        }
        saveName = name;
        saveVer = version;

        item->setIcon(icon);
        item->setText(saveEntry + " (version " + version + ")");
        item->setToolTip(saveEntry);
        ui->listWidget_List->addItem(item);

        currFiles << saveEntry;
    }
    latest += "latest " + saveName + " = " + saveVer + "\n";
    names << saveName; versions << saveVer;
    if (latestOpt == true && !files.isEmpty()) {
        getLatest(names, versions);
    }
    if (!currFiles.isEmpty() && debugOpt == true) {
        QInputDialog debug;
        debug.setOption(QInputDialog::UsePlainTextEditForTextInput);
        debug.setWindowTitle("Debug");
        debug.setLabelText("Debug info");
        debug.setMinimumSize(400, 400);
        debug.setTextValue(stuff + "\n\n\n\n" + latest);
        debug.setStyleSheet(stylesheet);
        debug.exec();
    }

    foreach (QString entry, folders) {
        QListWidgetItem* item = new QListWidgetItem();
        QIcon icon; icon = QIcon(":/icons/folder.png");

        item->setIcon(icon);
        item->setText(entry);
        item->setToolTip(entry);
        ui->listWidget_List->addItem(item);
    }
}
void MainWindow::getLatest(QStringList names, QStringList versions)
{
    QStringList latest;
    int i = 0;
    foreach(QString entry, currFiles) {
        int stop = names.count();
        if (i == stop) {break;}

        QString name = names.at(i);
        QString version = versions.at(i);

        if (entry.contains(name) && entry.contains(version)) {
            i += 1;
            latest << entry;
        }
    }

    ui->listWidget_List->clear();
    QListWidgetItem* up = new QListWidgetItem();
    QIcon icon; icon = QIcon(":/icons/folder.png");
    up->setIcon(icon);
    up->setText("Parent directory");
    ui->listWidget_List->addItem(up);

    foreach(QString entry, latest) {
        QListWidgetItem* item = new QListWidgetItem();
        icon = QIcon(":/icons/file.png");
        item->setIcon(icon);
        item->setText(entry);
        item->setToolTip(entry);
        ui->listWidget_List->addItem(item);
    }
    currFiles = latest;
}

void MainWindow::on_pushButton_Sort_clicked()
{
    thread.setFunction("updateList");
    thread.start();
    dl.getList();
}

void MainWindow::on_listWidget_List_itemDoubleClicked(QListWidgetItem *item)
{
    QString curr = item->toolTip();
    if (ui->listWidget_List->currentRow() == 0) {
        QStringList prepare = url.split("/"); prepare.removeAll("");
        QString remove = prepare.last();
        QString check = url.left(url.count() - remove.count() - 1);
        if (check == "http://" || check == "https://") {return;} else {url = check;}

        update(url);
        return;
    }
    if (curr.contains(".deb")) {
        if (path == "") {
            QMessageBox noPath;
            noPath.setStyleSheet(stylesheet);
            noPath.setWindowTitle("No download path");
            noPath.setText("No download path has been set.\n"
                           "Please set it by clicking on Set download path.");
            noPath.exec();
            return;
        }
        QDir check(path);
        if (!check.exists()) {
            QMessageBox badPath;
            badPath.setStyleSheet(stylesheet);
            badPath.setWindowTitle("Bad path");
            badPath.setText("The download path that has been set doesn't exist!\n"
                            "Please choose another path (click on Set download path).\n\n"
                            "If you want to recreate it, your path was:\n" + path);
            badPath.exec();
            return;
        }

        QStringList get = url.split("/"); get.removeAll("");
        QString folder = get.last();
        dl.setPath(path + folder + "/");
        QDir create; create.mkdir(path + folder);

        thread.setFunction("dlOne");
        thread.start();
        thread.sendInfo(curr);
        dl.startDownload(curr);
        prog.exec();
    } else {
        url += curr + "/";
        qDebug() << url;

        update(url);
    }
}

void MainWindow::on_pushButton_Download_clicked()
{
    if (currFiles.empty()) {
        QMessageBox nothing;
        nothing.setStyleSheet(stylesheet);
        nothing.setWindowTitle("No file found");
        nothing.setText("No .deb file has been detected in the folder.\n"
                        "Please choose another folder and press Download All.");
        nothing.exec();
        return;
    }
    if (path == "") {
        QMessageBox noPath;
        noPath.setStyleSheet(stylesheet);
        noPath.setWindowTitle("No download path");
        noPath.setText("No download path has been set.\n"
                       "Please set it by clicking on Set download path.");
        noPath.exec();
        return;
    }
    QDir check(path);
    if (!check.exists()) {
        QMessageBox badPath;
        badPath.setStyleSheet(stylesheet);
        badPath.setWindowTitle("Bad path");
        badPath.setText("The download path that has been set doesn't exist!\n"
                        "Please choose another path (click on Set download path).\n\n"
                        "If you want to recreate it, your path was:\n" + path);
        badPath.exec();
        return;
    }

    QStringList get = url.split("/"); get.removeAll("");
    QString folder = get.last();
    dl.setPath(path + folder + "/");
    QDir create; create.mkdir(path + folder);

    dl.setUrl(url);
    thread.setFunction("dlList");
    thread.receiveFiles(currFiles);
    thread.start();
    prog.exec();
}

void MainWindow::stopDl()
{
    thread.terminate();
    dl.stop();
    prog.unpack(QStringList() << "Download canceled" << "100" << "Download canceled" << "100");
}

void MainWindow::on_pushButton_Progress_clicked()
{
    prog.exec();
}

void MainWindow::on_pushButton_Path_clicked()
{
    QFileDialog getPath;
    QString viewPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    getPath.setDirectory(viewPath);
    getPath.setWindowTitle("Select download path");
    getPath.setFileMode(QFileDialog::Directory);
    QString path;
    if (getPath.exec() == QFileDialog::Accepted) {
        path = getPath.selectedFiles().join("");
        setPath(path);
        QMessageBox sel;
        sel.setStyleSheet(stylesheet);
        sel.setWindowTitle("Folder selected");
        sel.setText("\"" + path + "\" has been selected as a folder where this program will put downloaded files.\n"
                    "Your option will be saved.\n"
                    "To change it, click on Set download path.");
        sel.exec();
    }
}

void MainWindow::on_pushButton_Options_clicked()
{
    QStringList save = opts.get();
    if (opts.exec() == QDialog::Accepted) {
        setOptions(opts.get());
        update(url);
    } else {
        opts.set(save);
    }
}
