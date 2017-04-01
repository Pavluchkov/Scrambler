#include "windowlicense.h"
#include "ui_windowlicense.h"
#include "QMessageBox"

windowLicense::windowLicense(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::windowLicense)
{
    ui->setupUi(this);

    QRegExp rx("[A-Z-]+");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_key->setValidator(validator);
}

windowLicense::~windowLicense()
{
    delete ui;
}

void windowLicense::on_pushButton_ok_clicked()
{
    QString keyString = ui->lineEdit_key->text();

//        if(keyString == "123"){
//            license->setValue("True", 1);
//            licenseFlag = true;
//            QMessageBox::information(0, "Information", "Лицензионный ключ принят!");
//        }
//        else{
//            QMessageBox::critical(0, "Error", "Вы ввели неверный ключ!");
//            setLicense();
//        }
//    }
}
