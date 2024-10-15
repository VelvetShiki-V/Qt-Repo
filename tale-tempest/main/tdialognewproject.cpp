#include "tdialognewproject.h"
#include "ui_tdialognewproject.h"

TDialogNewProject::TDialogNewProject(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TDialogNewProject)
{
    ui->setupUi(this);
}

TDialogNewProject::~TDialogNewProject()
{
    delete ui;
}

void TDialogNewProject::on_buttonBox_accepted()
{
    QString name = ui->lineEdit->text();
    emit project_name_apply(name);
    accept();
}

