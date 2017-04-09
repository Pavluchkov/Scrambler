#include "windowlicense.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "QFileDialog"
#include "QMessageBox"
#include "QInputDialog"
#include "smbios.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //license = new QSettings(this);
    license = new QSettings("d:\\configuration.ini", QSettings::IniFormat, this);
    trialEnd = 3;
    trialTrue = 0;
    licenseFlag = false;
    testLicense();
    key = 0;

    QRegExp rx("\\d{9}[0-9]");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_key->setValidator(validator);

    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(disableLineEditKey()));
    connect(ui->comboBox_scrambler, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChange()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::disableLineEditKey()
{
    if(ui->checkBox->isChecked()){
        oldText = ui->lineEdit_key->text();
        //QMessageBox::warning(0,"Warning", QString("Key = %1").arg(key));
        ui->lineEdit_key->setText("Привязка к железу...");
        ui->lineEdit_key->setDisabled(true);
    }
    else{
        ui->lineEdit_key->setText(oldText);
        if(ui->comboBox_scrambler->currentIndex() < 1)
            ui->lineEdit_key->setDisabled(false);
    }
}

void Dialog::comboChange()
{
    ui->checkBox->setChecked(false);
    ui->lineEdit_key->setText("");

    if(ui->comboBox_scrambler->currentIndex() > 0){
        ui->lineEdit_key->setDisabled(true);
    }
    else ui->lineEdit_key->setDisabled(false);
}

void Dialog::testLicense()
{
    trialEnd = license->value("End", trialEnd);
    trialTrue = license->value("True", trialTrue);

    if(trialTrue.toInt()){
        licenseFlag = true;
        return;
    }

    if (trialEnd.toInt() != 0){
        licenseFlag = true;
        int n = trialEnd.toInt() - 1;
        license->setValue("End", n);

        int a = QMessageBox::warning(0, "Warning",
                                     "Вы используете Trial версию приложения!\n"
                                     "Срок действия пробного периода истекает\n"
                                     "            через " + trialEnd.toString() + " запуска приложения!\n"
                                                                                  "        Желаете ввести ключ лицензии?", QMessageBox::Yes | QMessageBox::No );
        if(a == QMessageBox::Yes)
            setLicense();
    }
    else {
        licenseFlag = false;
        setLicense();
    }

}

void Dialog::setLicense()
{
    windowLicense* win = new windowLicense;
    QObject::connect(win, SIGNAL(goodby()), win, SLOT(close()));
    win->exec();

    if(win->flag){
        license->setValue("True", 1);
        licenseFlag = true;
        QMessageBox::information(0, "Information", "Лицензионный ключ принят!");
    }
    delete win;
}

void Dialog::scrambler_xor()
{
    for(int i = 0; i < buffer.length(); i++){
        buffer[i] = buffer[i] ^ key;
    }
}

void Dialog::simpleRep()
{
    for(int i = 0; i < buffer.length(); i++){
        buffer[i] = (buffer[i] + 5) ^ key;
    }
}

void Dialog::deSimpleRep()
{
    for(int i = 0; i < buffer.length(); i++){
        buffer[i] = (buffer[i] ^ key) - 5;
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
    QFile file(fileName);
    QString m = ui->comboBox_mode->currentText();
    QString mode_Scrambler = ui->comboBox_scrambler->currentText();
    SMBios smbiosInfo;
    bool mode = false;

    if(m == "Шифрование")
        mode = true;

    if(fileName.isEmpty()){
        QMessageBox::warning(0, "Warning",
                             (mode)? "Укажите шифруемый файл!":"Укажите дешифруемый файл!");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(0, "Error", "Ошибка открытия файла !");
        return;
    }

    buffer = file.readAll();
    file.close();

    QString temp = fileName;

    fileName = ui->lineEdit_Save->text();

    if(fileName == temp){
        QMessageBox::warning(0, "Warning", "Входной и выходной файл совпадают.");
        return;
    }

    if(fileName.isEmpty()){
        QMessageBox::warning(0,"Warning", "Укажите выходной файл!");
        return;
    }

    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(0, "Error", "Ошибка создания файла !");
        return;
    }

    QString k = ui->lineEdit_key->text();
    int temp1 = ui->comboBox_scrambler->currentIndex();

    if(k == "Привязка к железу...")
        key = smbiosInfo.CPU.ID.toULongLong();
    else
        key = k.toLongLong();

    if((k.isEmpty()) && (temp1 < 1)){
        QMessageBox::warning(0,"Warning", "Введите ключ шифрования!");
        return;
    }

    //QMessageBox::warning(0,"Warning", QString("Key = %1").arg(key));

    if(mode_Scrambler == "Операция сложения по модулю 2"){
        if(mode){
            scrambler_xor();
        }
        else scrambler_xor();
    }

    if(mode_Scrambler == "Одноалфавитный шифр"){
        if(mode){
            simpleRep();
        }
        else deSimpleRep();
    }

    file.write(buffer);

    file.close();
    QMessageBox::information(0, "Information",
                             (mode)? "Файл успешно зашифрован !":"Файл успешно расшифрован !");
}
