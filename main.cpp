#include "dialog.h"
#include "license.h"
#include "smbios.h"
#include "QMessageBox"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    license lc("d:\\test.dat");

//    std::vector<std::string> r;
//    GetSMBInfo(0, r);
//    std::string str = r[0];
//    QString qs = QString::fromLocal8Bit(str.c_str());

    //QVariant str = r[0];
    //QMessageBox::information(0,"test", qs);

    Bios bios;
    bios.getInfo();
    QMessageBox::information(0,"test", bios.Vendor);

    if(lc.testLicense()){
        Dialog w;
        w.show();

        return a.exec();

    }
    a.exit();
}
