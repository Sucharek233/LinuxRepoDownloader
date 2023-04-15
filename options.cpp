#include "options.h"
#include "ui_options.h"

options::options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::options)
{
    ui->setupUi(this);

//    on_lineEdit_Repo_textChanged(ui->lineEdit_Repo->text());
//    on_checkBox_Latest_toggled(ui->checkBox_Latest->isChecked());
//    on_checkBox_Arch_toggled(ui->checkBox_Arch->isChecked());
}

options::~options()
{
    delete ui;
}

void options::set(QStringList options)
{
    repo = options.at(0);
    latest = options.at(1).toInt();
    arch = options.at(2).toInt();

    ui->lineEdit_Repo->setText(repo);
    ui->checkBox_Latest->setChecked(latest);
    ui->checkBox_Debug->setChecked(arch);
}
QStringList options::get()
{
    QStringList all;
    all << repo;
    all << QString::number(latest);
    all << QString::number(arch);
    return all;
}

void options::on_lineEdit_Repo_textChanged(const QString &arg1)
{
    repo = arg1;
}

void options::on_checkBox_Latest_toggled(bool checked)
{
    latest = checked;
}

void options::on_checkBox_Debug_toggled(bool checked)
{
    arch = checked;
}

void options::on_pushButton_Save_clicked()
{
    accept();
}

void options::on_pushButton_Cancel_clicked()
{
    reject();
}

void options::on_pushButton_Reset_clicked()
{
    ui->lineEdit_Repo->setText("http://archive.ubuntu.com/ubuntu/pool/");
    ui->checkBox_Latest->setChecked(0);
    ui->checkBox_Debug->setChecked(0);
}
