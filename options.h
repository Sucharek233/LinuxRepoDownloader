#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class options;
}

class options : public QDialog
{
    Q_OBJECT

public:
    explicit options(QWidget *parent = nullptr);
    ~options();

    void set(QStringList options);
    QStringList get();

private slots:
    void on_lineEdit_Repo_textChanged(const QString &arg1);

    void on_checkBox_Latest_toggled(bool checked);

    void on_checkBox_Debug_toggled(bool checked);

    void on_pushButton_Save_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_Reset_clicked();

private:
    Ui::options *ui;

    QString repo;
    bool latest;
    bool arch;
};

#endif // OPTIONS_H
