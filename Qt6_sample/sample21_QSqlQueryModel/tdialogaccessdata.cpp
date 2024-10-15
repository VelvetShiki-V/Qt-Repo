#include "tdialogaccessdata.h"
#include "ui_tdialogaccessdata.h"

TDialogAccessData::TDialogAccessData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogAccessData)
{
    ui->setupUi(this);
}

TDialogAccessData::~TDialogAccessData()
{
    delete ui;
}

void TDialogAccessData::set_record(const QSqlRecord &record)
{
    m_record = record;
    ui->lineEdit->setText(record.value("name").toString());
    ui->plainTextEdit->appendPlainText(record.value("memo").toString());
}

QSqlRecord TDialogAccessData::get_record()
{
    m_record.setValue("name", ui->lineEdit->text());
    m_record.setValue("memo", ui->plainTextEdit->toPlainText());
    return m_record;
}
