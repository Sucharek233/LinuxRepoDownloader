#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "thread.h"
#include "download.h"
#include "progress.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
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

    void sendOutput(QString out) {thread.receiveOut(out);}
    void sendDlOutput(QString out) {thread.receiveDlOut(out);}
    void sendState(bool state) {thread.receiveState(state);}

    void stopDl();

    void initPath();
    void setPath(QString newPath);

private slots:
    void updateList(QStringList folders, QStringList files);

    void on_pushButton_Sort_clicked();

    void on_listWidget_List_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_Download_clicked();

    void on_pushButton_Progress_clicked();

    void on_pushButton_Path_clicked();

private:
    Ui::MainWindow *ui;

    Thread thread;
    download dl;
    progress prog;

    QString url = "http://archive.ubuntu.com/ubuntu/pool/";

    QString path;
};
#endif // MAINWINDOW_H
