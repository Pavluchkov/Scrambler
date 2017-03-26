#include "dialog.h"
#include "license.h"
#include "QMessageBox"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    license lc("d:\\test.dat");

//    SMBios smbiosInfo;

//    QMessageBox::information(0,"Battery", smbiosInfo.PortableBattery.Date);
//    QMessageBox::information(0,"Processor", smbiosInfo.CPU.SocketDesignation);
    if(lc.testLicense()){
        Dialog w;
        w.show();

        return a.exec();

    }
    a.exit();
}
