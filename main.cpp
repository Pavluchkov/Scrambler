#include "dialog.h"
#include "license.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    license lc("c:\\test.dat");

    if(lc.testLicense()){
        Dialog w;
        w.show();

        return a.exec();

    }
    a.exit();
}
