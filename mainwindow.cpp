#include "mainwindow.h"
#include "ui_mainwindow.h"

QString stylesheet;

QStringList currFiles;

QString configPath;

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

    update(url);
    initPath();

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

void MainWindow::initPath()
{
    configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    QDir checkFolder(configPath); if (!checkFolder.exists()) {checkFolder.mkdir(configPath);}
    QFile config(configPath + "config.ini"); if (!config.exists()) {config.open(QIODevice::ReadWrite); config.close();}

    config.open(QIODevice::ReadOnly);
    path = config.readLine();
    config.close();

    dl.setPath(path);
}

void MainWindow::setPath(QString newPath)
{
    QFile config(configPath + "config.ini");
    config.open(QIODevice::ReadWrite);
    config.resize(0);
    config.write(newPath.toUtf8() + "/");
    config.close();

    initPath();
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

    foreach (QString entry, files) {
        QListWidgetItem* item = new QListWidgetItem();
        QIcon icon; icon = QIcon(":/icons/file.png");

        item->setIcon(icon);
        item->setText(entry);
        ui->listWidget_List->addItem(item);
        currFiles << entry;
    }

    foreach (QString entry, folders) {
        QListWidgetItem* item = new QListWidgetItem();
        QIcon icon; icon = QIcon(":/icons/folder.png");

        item->setIcon(icon);
        item->setText(entry);
        ui->listWidget_List->addItem(item);
    }
}

void MainWindow::on_pushButton_Sort_clicked()
{
    thread.setFunction("updateList");
    thread.start();
    dl.getList();
}

void MainWindow::on_listWidget_List_itemDoubleClicked(QListWidgetItem *item)
{
    QString curr = item->text();
    if (ui->listWidget_List->currentRow() == 0) {
        if (url == "http://archive.ubuntu.com/") {return;}
        QStringList prepare = url.split("/"); prepare.removeAll("");
        QString remove = prepare.last();
        url = url.left(url.count() - remove.count() - 1);

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
