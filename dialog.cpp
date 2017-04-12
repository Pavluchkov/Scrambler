#include "windowlicense.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "QFileDialog"
#include "QMessageBox"
#include "QtAlgorithms"
#include "algorithm"
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

void Dialog::simpleRep(bool flag)
{
    for(int i = 0; i < buffer.length(); i++){

        if(flag){
            buffer[i] = (buffer[i] + 5) ^ key;
        }
        else buffer[i] = (buffer[i] ^ key) - 5;
    }
}

void Dialog::omofChange(bool flag)
{
    char temp;

    if(flag){

        for(int i = 0; i < buffer.length(); i+= 2){
            temp = (buffer[i] + 2) ^ key;
            buffer[i] = temp;
            temp = (buffer[i] - 4) ^ key;
            buffer.insert(i + 1, temp);
        }
    }
    else{

        for(int i = 0; i < buffer.length(); i++){
            buffer[i] = (buffer[i] ^ key) - 2;
            buffer.remove(i + 1, 1);
        }
    }
}

void Dialog::blockChange(bool flag)
{
    QVector<int> index;
    int indexPoint = 1;
    int step = 3;
    int count = 0;
    int hvost = buffer.length() % 3;

    QByteArray strFind;
    QByteArray strChange;

    for(int i = 0; i < buffer.length() - hvost; i += step){

        strFind.clear();
        strChange.clear();

        if(!index.isEmpty()){

            qCount(index.begin(), index.end(), i, count);

            if(count){

                step = 3;
                count = 0;
                ++indexPoint;
                //QMessageBox::information(0,"",QString("IndexPoint = %1").arg(indexPoint));
                continue;
            }
            if(indexPoint < index.length()){

                if((step = index[indexPoint] - i) < 3)
                    if(step > 0)
                        continue;
            }
            step = 3;
        }

        //QMessageBox::information(0,"", QString(" i = %1\n").arg(i) +
        //QString(" indexPoint = %1\n").arg(indexPoint) +
        //QString(" Step = %1\n").arg(step) + buffer);


        strFind.push_back(buffer[i]);
        strFind.push_back(buffer[i+1]);
        strFind.push_back(buffer[i+2]);

        strChange.push_back(strFind[2]);
        //char c = (strFind[1]/* + 1*/) ^ key;
        strChange.push_back(strFind[1]/* ^ key*/);
        strChange.push_back(strFind[0]);


        int j = i;
        //buffer.replace(j, 3, strChange);
        while((j = buffer.indexOf(strFind, j)) != -1){

            index.push_back(j);
            buffer.replace(j, 3, strChange);
            j += 3;
        }

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

    QString m = ui->comboBox_mode->currentText();
    QString mode_Scrambler = ui->comboBox_scrambler->currentText();
    SMBios smbiosInfo;
    bool mode = false;

    if(m == "Шифрование")
        mode = true;

    if(ui->lineEdit_Load->text().isEmpty()){
        QMessageBox::warning(0, "Warning",
                             (mode)? "Укажите шифруемый файл!":"Укажите дешифруемый файл!");
        return;
    }

    if(ui->lineEdit_Save->text().isEmpty()){
        QMessageBox::warning(0,"Warning", "Укажите выходной файл!");
        return;
    }

    if(ui->lineEdit_Load->text() == ui->lineEdit_Save->text()){
        QMessageBox::warning(0, "Warning", "Входной и выходной файл совпадают.");
        return;
    }

    QString k = ui->lineEdit_key->text();
    int temp = ui->comboBox_scrambler->currentIndex();

    if((k.isEmpty()) && (temp < 1)){
        QMessageBox::warning(0,"Warning", "Введите ключ шифрования!");
        return;
    }

    if(k == "Привязка к железу...")
        key = smbiosInfo.CPU.ID.toULongLong();
    else
        key = k.toLongLong();

    QString fileName = ui->lineEdit_Load->text();
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(0, "Error", "Ошибка открытия файла !");
        return;
    }

    buffer = file.readAll();
    file.close();

    fileName = ui->lineEdit_Save->text();
    file.setFileName(fileName);

    if (!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(0, "Error", "Ошибка создания файла !");
        return;
    }

    //QMessageBox::warning(0,"Warning", QString("Key = %1").arg(key));

    if(mode_Scrambler == "Операция сложения по модулю 2"){
        scrambler_xor();
    }

    if(mode_Scrambler == "Одноалфавитный шифр"){
        simpleRep(mode);
    }

    if(mode_Scrambler == "Омофонная замена (1х2)"){
        omofChange(mode);
    }

    if(mode_Scrambler == "Блочная замена (3х3)"){
        blockChange(mode);
    }

    file.write(buffer);

    file.close();
    QMessageBox::information(0, "Information",
                             (mode)? "Файл успешно зашифрован !":"Файл успешно расшифрован !");
}
