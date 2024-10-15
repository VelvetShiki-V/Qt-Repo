#include "tdialogroledata.h"
#include "ui_tdialogroledata.h"

TDialogRoleData::TDialogRoleData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogRoleData)
{
    ui->setupUi(this);
}

TDialogRoleData::~TDialogRoleData()
{
    delete ui;
}

void TDialogRoleData::set_record(const QSqlRecord &record)
{
    m_record = record;
    ui->lineEdit->setText(record.value("name").toString());
    ui->plainTextEdit->appendPlainText(record.value("memo").toString());
}

QSqlRecord TDialogRoleData::get_record()
{
    m_record.setValue("name", ui->lineEdit->text());
    m_record.setValue("memo", ui->plainTextEdit->toPlainText());
    return m_record;
}
