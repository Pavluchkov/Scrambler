#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public:
    void scrambler_xor();

private slots:
    void on_toolButton_Load_clicked();

    void on_toolButton_Save_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_Ok_clicked();

private:
    Ui::Dialog *ui;

    QByteArray          buffer;
    unsigned long long   key;
};

#endif // DIALOG_H
