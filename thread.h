#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>

class Thread : public QThread
{
   Q_OBJECT

public:
    void setFunction(QString func) {function = func;}

    void receiveFiles(QStringList dlFiles) {files = dlFiles;}

    void sendInfo(QString info) {fileName = info;}

    QStringList stripFiles(QString input);
    QStringList stripFolders(QString input);

    QString getPercentage(QString input);

    QString getSizes();

public slots:
    void receiveList(QString out) {output = out;}
    void receiveState(bool stat) {state = stat;}
    void receiveInfo(QStringList inf) {info = inf;}
    void receiveError(QString error) {err = error;}

protected:
   virtual void run() override;

signals:
   void update(QStringList, QStringList);
   void func(QString);

   void updateProgress(QStringList);

   void sendFile(QString);

   void showError(QStringList);

private:
    QString function;

    bool state;

    QString output;
    QStringList info;
    QString fileName;
    QString err;

    QStringList files;
};

#endif // THREAD_H
