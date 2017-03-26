#include "dialog.h"
#include "ui_dialog.h"
#include "QFileDialog"
#include "QMessageBox"
#include "smbios.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    key = 0;
    QRegExp rx("\\d{9}[0-9]");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_key->setValidator(validator);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::scrambler_xor()
{
    //QMessageBox::information(0,"test","Процедура шифрования");
    for(int i = 0; i < buffer.length(); i++){
        buffer[i] = buffer[i] ^ key;
    }

}

void Dialog::on_toolButton_Load_clicked()
{
    QString strLoad = QFileDialog::getOpenFileName(this, "Open File", "", "");
    ui->lineEdit_Load->setText(strLoad);
}

void Dialog::on_toolButton_Save_clicked()
{
    QString strSave = QFileDialog::getSaveFileName(this, "Save File", "", "");
    ui->lineEdit_Save->setText(strSave);
}

void Dialog::on_pushButton_Cancel_clicked()
{
    QApplication::quit();
}

void Dialog::on_pushButton_Ok_clicked()
{
    QString fileName = ui->lineEdit_Load->text();
    QString k = ui->lineEdit_key->text();
    QFile file(fileName);
    QString m = ui->comboBox_mode->currentText();
    QString mode_Scrambler = ui->comboBox_scrambler->currentText();
    SMBios smbiosInfo;
    bool mode = false;

    if(ui->checkBox->isChecked())
        key = smbiosInfo.CPU.ID.toULongLong();
    else
        if(!k.isEmpty())
            key = k.toULongLong();

    if(m == "Шифрование")
        mode = true;

    if(fileName.isEmpty()){
        QMessageBox::warning(0, "Warning",
                             (mode)? "Выберите шифруемый файл!":"Выберите дешифруемый файл!");
        return;
    }


    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(0, "Error", "Ошибка открытия файла !");
        return;
    }

    buffer = file.readAll();
    file.close();

    fileName = ui->lineEdit_Save->text();

    if(fileName.isEmpty()){
        QMessageBox::warning(0, "Warning",
                             (mode)? "Укажите куда шифровать файл!":"Укажите куда дешифровать файл!");
        return;
    }

    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(0, "Error", "Ошибка создания файла !");
        return;
    }

    if(mode_Scrambler == "Операция сложения по модулю 2"){
        if(mode){
            scrambler_xor();
        }
        else scrambler_xor();
    }

    file.write(buffer);

    file.close();
    QMessageBox::information(0, "Information",
                             (mode)? "Файл успешно зашифрован !":"Файл успешно расшифрован !");
}
