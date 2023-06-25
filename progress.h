#ifndef PROGRESS_H
#define PROGRESS_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class progress;
}

class progress : public QDialog
{
    Q_OBJECT

public:
    explicit progress(QWidget *parent = nullptr);
    ~progress();

    void test();

    void currLabel(QString text);
    void currPercentage(int percentage);

    void overallLabel(QString text);
    void overallPercentage(int percentage);

public slots:
    void unpack(QStringList data);

private slots:
    void on_pushButton_Cancel_clicked();

    void on_pushButton_Close_clicked();

signals:
    void stopDl();

private:
    Ui::progress *ui;
};

#endif // PROGRESS_H
