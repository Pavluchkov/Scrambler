#include "windowlicense.h"
#include "ui_windowlicense.h"
#include "QMessageBox"

windowLicense::windowLicense(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::windowLicense)
{
    ui->setupUi(this);

    flag = false;
    QRegExp rx("[A-Z]{4}-[A-Z]{4}-[A-Z]{4}-[A-Z]{4}");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_key->setValidator(validator);
}

windowLicense::~windowLicense()
{
    delete ui;
}

void windowLicense::on_pushButton_ok_clicked()
{
    QString temp;
    int sum = 0;

    temp = ui->lineEdit_key->text();

    for(auto i:temp){
        if(i != '-')
            sum += i.unicode();
    }
    //QMessageBox::information(0,"", QString("arg(sum)%1").arg(sum));
    if(sum == 1200){
        flag = true;
        emit goodby();
    }
    else
        QMessageBox::critical(0, "Error", "Вы ввели неверный ключ!");
}

void windowLicense::on_pushButton_cancel_clicked()
{
    emit goodby();
}
