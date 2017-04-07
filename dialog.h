#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "QSettings"
#include "windowlicense.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    void testLicense();
    void setLicense();

    void scrambler_xor();
    void simpleRep();
    void deSimpleRep();

private slots:
    void on_toolButton_Load_clicked();

    void on_toolButton_Save_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_Ok_clicked();

    void disableLineEditKey();

private:
    Ui::Dialog *ui;

    QSettings*          license;
    QVariant            trialEnd;
    QVariant            trialTrue;
    QByteArray          buffer;
    QString             oldText;
    unsigned long long   key;

public:
    bool                licenseFlag;

};

#endif // DIALOG_H
