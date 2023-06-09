#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "thread.h"
#include "download.h"
#include "progress.h"
#include "options.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void update(QString url);
    void function(QString func);

    void stopDl();

    void initInfo();
    void setPath(QString newPath);
    void setOptions(QStringList options);

    void threadDl(QString file);

    void error(QStringList err);

private slots:
    void updateList(QStringList folders, QStringList files);
    void getLatest(QStringList names, QStringList versions);

    void on_pushButton_Sort_clicked();

    void on_listWidget_List_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_Download_clicked();

    void on_pushButton_Progress_clicked();

    void on_pushButton_Path_clicked();

    void on_pushButton_Options_clicked();

private:
    Ui::MainWindow *ui;

    Thread thread;
    download dl;
    progress prog;
    options opts;

    QString url = "http://archive.ubuntu.com/ubuntu/pool/";

    QString path;
};
#endif // MAINWINDOW_H
