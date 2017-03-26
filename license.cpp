#include "license.h"
#include "QFile"
#include "QMessageBox"
#include "QInputDialog"

license::license(QString str)
{
    licenseFileName.setFileName(str);

    if(!licenseFileName.exists()){
        if(!licenseFileName.open(QIODevice::WriteOnly)){
            return;
        }
        licenseFileName.write("300");

        licenseFileName.close();
    }
}

bool license::testLicense()
{

    if(!licenseFileName.open(QIODevice::ReadWrite)){
        QMessageBox::critical(0, "Error", "Ошибка открытия файла лицензии!");
        return false;
    }

    char l[3];

    while(!licenseFileName.atEnd()){
        licenseFileName.read(l, sizeof(l));
    }

    licenseFileName.close();

    if(l[2] != '0'){
        return true;
    }

    if(l[1] >= l[0]){

        return setLicense();
    }

    l[1]++;

    QVariant n = l[0] - l[1];
    int a = QMessageBox::warning(0, "Information", "Вы используете Trial версию приложения!\n"
                                                   "Срок действия пробного периода истекает\n"
                                                   " через " + n.toString() +" запуска приложения!\n"
                                                                             "Желаете ввести ключ лицензии?",
                                 QMessageBox::Yes | QMessageBox::No );
    if(a == QMessageBox::Yes)
        return setLicense();

    if(!licenseFileName.open(QIODevice::WriteOnly)){
        QMessageBox::critical(0, "Error", "Ошибка открытия файла лицензии!");
        return false;
    }

    licenseFileName.write(l, sizeof(l));
    licenseFileName.close();
    return true;
}

bool license::setLicense()
{

    bool Ok;
    QString str = QInputDialog::getText(0, "Input", "Пробный период закончен! \n"
                                                    "Введите лицензионный ключ: ",
                                        QLineEdit::Normal, "Девятизначный ключ", &Ok);
    if(!Ok){
        return false;
    }

    if(str == "123"){
        QMessageBox::information(0, "Error", "Лицензионный ключ принят!");

        if(!licenseFileName.open(QIODevice::WriteOnly)){
            QMessageBox::critical(0, "Error", "Ошибка открытия файла лицензии!");
            return false;
        }

        licenseFileName.write("111", sizeof("111"));
        licenseFileName.close();
        return true;
    }
    else{
        QMessageBox::critical(0, "Error", "Вы ввели неверный ключ!");
        setLicense();
    }
    return false;
}
