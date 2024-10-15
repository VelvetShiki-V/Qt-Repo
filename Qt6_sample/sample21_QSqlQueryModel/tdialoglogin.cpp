#include "tdialoglogin.h"
#include "ui_tdialoglogin.h"

TDialogLogin::TDialogLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogLogin)
{
    ui->setupUi(this);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
}

TDialogLogin::~TDialogLogin()
{
    delete ui;
}

QString TDialogLogin::user()
{
    return ui->lineEdit_user->text();
}

QString TDialogLogin::password()
{
    return ui->lineEdit_pwd->text();
}

QString TDialogLogin::ip()
{
    return ui->lineEdit_ip->text();
}
