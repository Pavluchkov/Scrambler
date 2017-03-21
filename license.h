#ifndef LICENSE_H
#define LICENSE_H

#include "QString"
#include "QFile"


class license
{
public:
    license(QString str = "c:\\licence.dat");
    bool setLicense();
    bool testLicense();

private:
    QFile licenseFileName;
};

#endif // LICENSE_H
