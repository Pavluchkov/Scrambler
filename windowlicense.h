#ifndef WINDOWLICENSE_H
#define WINDOWLICENSE_H

#include <QDialog>
#include "dialog.h"

namespace Ui {
class windowLicense;
}

class windowLicense : public QDialog
{
    Q_OBJECT

public:
    explicit windowLicense(QWidget *parent = 0);
    ~windowLicense();

private slots:
    void on_pushButton_ok_clicked();

private:
    Ui::windowLicense *ui;
};

#endif // WINDOWLICENSE_H
