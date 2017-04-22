#include "windowlicense.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "QFileDialog"
#include "QMessageBox"
#include "time.h"
#include "QTextStream"
#include "QTextCodec"
#include "smbios.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //license = new QSettings(this); // используется для записи в реестр
    license = new QSettings("d:\\configuration.ini", QSettings::IniFormat, this);
    trialEnd = 3;
    trialTrue = 0;
    licenseFlag = false;
    testLicense();
    key = 0;

    QRegExp rx("[0-9]{8}");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_key->setValidator(validator);

    ui->checkBox->setToolTip(" Если этот параметр включен, то ключ шифрования\n"
                             "генерируется под конкретную машину, исключая тем самым\n"
                             "несанкционированную расшифровку файла на другом ПК.");

    ui->lineEdit_key->setToolTip("Ключ шифрования должен содержать\n"
                                 "только цифры не более 8 - ми знаков.");

    connect(ui->checkBox, SIGNAL(clicked(bool)), this, SLOT(disableLineEditKey()));
    connect(ui->comboBox_scrambler, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChange()));

    ui->lineEdit_Load->setText("C:\\Users\\pav_a\\Desktop\\test1.txt");
    ui->lineEdit_Save->setText("C:\\Users\\pav_a\\Desktop\\test2.txt");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::disableLineEditKey()
{
    if(ui->checkBox->isChecked()){
        oldText = ui->lineEdit_key->text();
        ui->lineEdit_key->setText("Привязка к железу...");
        ui->lineEdit_key->setDisabled(true);
    }
    else{
        ui->lineEdit_key->setText(oldText);

        if(ui->comboBox_scrambler->currentIndex() < 2)
            ui->lineEdit_key->setDisabled(false);
    }
}

void Dialog::comboChange()
{
    ui->checkBox->setChecked(false);
    ui->lineEdit_key->setText("");
    //ui->comboBox_mode->setCurrentIndex(0);

    if(ui->comboBox_scrambler->currentIndex() > 1){
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
                                     "  Вы используете Trial версию приложения!\n"
                                     "Срок действия пробного периода истекает\n"
                                     "через " + trialEnd.toString() + " запуск(а) приложения.\n\n"
                                                                      "\tЖелаете ввести ключ лицензии?", QMessageBox::Yes | QMessageBox::No );
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
            buffer[i] = (buffer[i] + key -3) ^ key;
        }
        else buffer[i] = (buffer[i] ^ key) - key + 3;
    }
}

bool Dialog::omofChange(bool flag)
{ 
    QString fileName_key;

    if(flag)
        fileName_key = ui->lineEdit_Save->text();
    else
        fileName_key = ui->lineEdit_Load->text();

    srand(time(0));

    if(flag){
        QByteArray symbol;

        for(int i = 0; i < buffer.length(); i++){
            if(buffer[i] == '\r')
                buffer.remove(i, 1);
            if(buffer[i] == '\n')
                buffer[i] = '\0';
        }

        for(int i = 0, count = 0; i < buffer.length(); count = 0, i++){

            qCount(symbol, buffer[i], count);

            if(!count){
                symbol.push_back(buffer[i]);
            }
        }

        int n = 0;
        int** rndValue = new int* [3];
        QList<int> rndList;

        for(int i = 0; i < 3; i++)
            rndValue[i] = new int[symbol.length()];

        for(int i = 0; i < 3; i++)

            for(int j = 0; j < symbol.length(); j++){
                n = rand() % (symbol.length() * 3 + 100) + 100;

                while(rndList.indexOf(n) != -1){
                    n = rand() % (symbol.length() * 3 + 100) + 100;
                }

                rndList.push_back(n);
                rndValue[i][j] = n;
            }

        n = 0;

        for(int i = 0; i < buffer.length(); i++){
            if((n = symbol.indexOf(buffer[i])) != -1){
                buffer.remove(i, 1);
                buffer.insert(i, QString::number(rndValue[rand() % 3][n]));
                i += 2;
            }
        }

        QString s[4];

        for(auto i : symbol)
            s[0] += i;

        for(int i = 0; i < 3; i++){

            for(int j = 0; j < symbol.length(); j++){
                s[i+1] += QString::number(rndValue[i][j]);
            }

        }

        int j = 0;

        j = fileName_key.indexOf(".", j);
        fileName_key.replace(j + 1, 3, "key");

        QFile f(fileName_key);

        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::critical(0, "Error", "Ошибка создания файла ключа!");
            return false;
        }

        QTextStream stream(&f);

        for(auto i : s){
            stream << i + '\n';
        }

        if(stream.status() != QTextStream::Ok){
            QMessageBox::critical(0, "Error", "Ошибка записи файла ключа!");
            return false;
        }

        f.close();
        QMessageBox::information(0, "Information", "В каталог с зашифрованным файлом\n"
                                                   "помещен файл ключей с расширением .key, \n"
                                                   "необходимый для дешифрации данных.");

        for(int i = 0; i < 3; i++)  // Удаление динамического двумерного массива
            delete []rndValue[i];
    }

    if(!flag){
        int j = 0;

        j = fileName_key.indexOf(".", j);
        fileName_key.replace(j + 1, 3, "key");

        QFile f(fileName_key);

        if(!f.exists()){
            QMessageBox::critical(0, "Error", "Отсутствует файл ключа, необходимый \n"
                                              "для корректной дешифрации данных!");
            return false;
        }

        if (!f.open(QIODevice::ReadOnly)){
            QMessageBox::critical(0, "Error", "Ошибка открытия файла ключа!");
            return false;
        }

        QMultiMap<char, int> map;
        QMultiMap<char, int>::iterator it;
        QByteArray str_temp;
        QByteArray volume[4];

        j = 0;

        while(!f.atEnd() && j < 4){
            volume[j] = f.readLine();
            j++;
        }

        f.close();

        int k = 0;
        str_temp.clear();

        for(int i = 0; i < volume[0].length(); i++){
            for(int j = 1; j < 4; j++){
                str_temp.push_back(volume[j][k]);
                str_temp.push_back(volume[j][k+1]);
                str_temp.push_back(volume[j][k+2]);

                map.insert(volume[0][i], str_temp.toInt());
                str_temp.clear();
            }
            k += 3;
        }

        str_temp.clear();
        QByteArray buffer_new;
        int hvost = buffer.length() % 3;

        for(int i = 0; i < buffer.length() - hvost; i += 3){
            str_temp.clear();
            str_temp.push_back(buffer[i]);
            str_temp.push_back(buffer[i+1]);
            str_temp.push_back(buffer[i+2]);

            it = map.begin();

            while(it != map.end()){

                if(it.value() == str_temp.toInt()){
                    if(it.key() == '\0'){
                        buffer_new.push_back('\r');
                        buffer_new.push_back('\n');
                    }
                    else buffer_new.push_back(it.key());
                }

                it++;
            }
        }

        buffer = buffer_new;
    }

    return true;
}

bool Dialog::blockChange(bool flag)
{
    QVector<int> index;
    int step = 3;
    int count = 0;
    int hvost = buffer.length() % 3;

    QByteArray strFind;
    QByteArray strChange;
    QString fileName_key;

    if(flag)
        fileName_key = ui->lineEdit_Save->text();
    else
        fileName_key = ui->lineEdit_Load->text();

    int j = 0;

    j = fileName_key.indexOf(".", j);
    fileName_key.replace(j + 1, 3, "key");

    QFile f(fileName_key);

    if(flag){

        for(int i = 0; i < buffer.length() - hvost; i += step){

            strFind.clear();
            strChange.clear();

            count = 0;
            step = 3;
            qCount(index, i, count);

            if(count != 0){
                continue;
            }

            for(auto k:index){
                if(k > i){
                    step = k - i;
                    break;
                }
            }

            if(step < 3)
                continue;
            else step = 3;

            strFind.push_back(buffer[i]);
            strFind.push_back(buffer[i+1]);
            strFind.push_back(buffer[i+2]);

            strChange.push_back(strFind[2]);
            strChange.push_back(strFind[1] ^ key);
            strChange.push_back(strFind[0]);

            int j = i;

            while((j = buffer.indexOf(strFind, j)) != -1){
                count = 0;
                qCount(index, j, count);
                qCount(index, j+1, count);
                qCount(index, j+2, count);
                qCount(index, j-1, count);
                qCount(index, j-2, count);

                if(count == 0){
                    index.push_back(j);
                    buffer.replace(j, 3, strChange);
                }

                j += 3;
            }

            qSort(index.begin(), index.end());
        }

        if (!f.open(QIODevice::WriteOnly)){
            QMessageBox::critical(0, "Error", "Ошибка создания файла ключа!");
            return false;
        }

        for(auto i : index)
            i = i ^ index.length();

        QDataStream stream(&f);
        stream << index;

        if(stream.status() != QDataStream::Ok){
            QMessageBox::critical(0, "Error", "Ошибка записи файла ключа!");
            return false;
        }

        f.close();
        QMessageBox::information(0, "Information", "В каталог с зашифрованным файлом\n"
                                                   "помещен файл ключей с расширением .key, \n"
                                                   "необходимый для дешифрации данных.");
    }

    if(!flag){

        if(!f.exists()){
            QMessageBox::critical(0, "Error", "Отсутствует файл ключа, необходимый \n"
                                              "для корректной дешифрации данных!");
            return false;
        }

        if (!f.open(QIODevice::ReadOnly)){
            QMessageBox::critical(0, "Error", "Ошибка открытия файла ключа!");
            return false;
        }

        QDataStream stream(&f);
        stream >> index;

        if(stream.status() != QDataStream::Ok){
            QMessageBox::critical(0, "Error", "Ошибка чтения файла ключа!");
            return false;
        }

        f.close();

        for(auto i : index)
            i = i ^ index.length();

        char temp;

        for(auto i:index){
            temp = buffer[i];
            buffer[i] = buffer[i+2];
            buffer[i+2] = temp;
            buffer[i+1] = buffer[i+1] ^ key;
        }
    }
    return true;
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

    if((k.isEmpty()) && (temp < 2)){
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

    if(mode_Scrambler == "Операция сложения по модулю 2"){
        scrambler_xor();
    }

    if(mode_Scrambler == "Одноалфавитный шифр"){
        simpleRep(mode);
    }

    if(mode_Scrambler == "Омофонная замена (1х3)"){
        if(!omofChange(mode))
            return;
    }

    if(mode_Scrambler == "Блочная замена (3х3)"){
        if(!blockChange(mode))
            return;
    }

    file.write(buffer);
    file.close();
    QMessageBox::information(0, "Information",
                             (mode)? "Файл успешно зашифрован !":"Файл успешно расшифрован !");
}
