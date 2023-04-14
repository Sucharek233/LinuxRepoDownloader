#include "progress.h"
#include "ui_progress.h"

progress::progress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progress)
{
    ui->setupUi(this);
}

progress::~progress()
{
    delete ui;
}

void progress::unpack(QStringList data)
{
    currLabel(data.at(0));
    currPercentage(data.at(1).toInt());
    overallLabel(data.at(2));
    overallPercentage(data.at(3).toInt());
}

void progress::currLabel(QString text)
{
    ui->label_Curr->setText(text);
}
void progress::currPercentage(int percentage)
{
    ui->progressBar_Curr->setValue(percentage);
}

void progress::overallLabel(QString text)
{
    ui->label_Overall->setText(text);
}
void progress::overallPercentage(int percentage)
{
    ui->progressBar_Overall->setValue(percentage);
}

void progress::on_pushButton_Cancel_clicked()
{
    int check = ui->progressBar_Overall->value();
    if (check == 100) {return;}
    stopDl();
}

void progress::on_pushButton_Close_clicked()
{
    close();
}
