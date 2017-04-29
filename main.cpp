#include "dialog.h"
#include "windowlicense.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("Scrambler");
    QApplication::setApplicationName("License");

    Dialog w;

    if(w.licenseFlag){
        w.show();
        return a.exec();
    }

    a.exit();
}
