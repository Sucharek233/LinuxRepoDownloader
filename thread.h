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

    void receiveOut(QString out) {output = out;}
    void receiveDlOut(QString out) {dlOutput = out;}
    void receiveState(bool stat) {state = stat;}

    void receiveFiles(QStringList dlFiles) {files = dlFiles;}

    void sendInfo(QString info) {dlInfo = info;}

    QStringList stripFiles(QString input);
    QStringList stripFolders(QString input);

    QString getPercentage(QString input);

public slots:
    void stopRunning();

protected:
   virtual void run();

signals:
   void update(QStringList, QStringList);
   void func(QString);

   void updateProgress(QStringList);

   void sendFile(QString);

private:
    bool isRunning;

    QString function;

    bool state;

    QString output;
    QString dlOutput;
    QString dlInfo;

    QStringList files;
};

#endif // THREAD_H
