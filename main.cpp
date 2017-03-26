#include "dialog.h"
#include "license.h"
#include "smbios.h"
#include "QMessageBox"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    license lc("d:\\test.dat");

    SMBios smbiosInfo;

    QMessageBox::information(0,"Processor", smbiosInfo.PortableBattery.Date);

    if(lc.testLicense()){
        Dialog w;
        w.show();

        return a.exec();

    }
    a.exit();
}
