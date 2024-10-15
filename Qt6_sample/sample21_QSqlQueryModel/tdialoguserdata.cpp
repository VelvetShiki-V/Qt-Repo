#include "tdialoguserdata.h"
#include "ui_tdialoguserdata.h"

TDialogUserData::TDialogUserData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogUserData)
{
    ui->setupUi(this);
}

TDialogUserData::~TDialogUserData()
{
    delete ui;
}

void TDialogUserData::set_record(const QSqlRelationalTableModel* model, const int& cur_row)
{
    // 下拉框初始化（外键为第二列role字段关联的role表）
    QSqlTableModel* relate = model->relationModel(foreign_role_column);
    for(int i = 0; i < relate->rowCount(); ++i) {
        ui->comboBox_role->addItem(relate->record(i).value("name").toString());
    }
    // 当前对话框内容设置
    m_record = model->record(cur_row);
    ui->lineEdit_name->setText(m_record.value("name").toString());
    ui->lineEdit_pwd->setText(m_record.value("password").toString());
    ui->comboBox_role->setCurrentText(m_record.value(foreign_role_name).toString());
    ui->dateEdit_create->setDate(m_record.value("createtime").toDate());
    ui->dateEdit_expire->setDate(m_record.value("expiration").toDate());
    ui->lineEdit_company->setText(m_record.value("company").toString());
    ui->lineEdit_tel->setText(m_record.value("mobile").toString());
    ui->plainTextEdit->appendPlainText(m_record.value("memo").toString());
}

QSqlRecord TDialogUserData::get_record()
{
    m_record.setValue("name", ui->lineEdit_name->text());
    m_record.setValue("password", ui->lineEdit_pwd->text());
    m_record.setValue(foreign_role_name, ui->comboBox_role->currentText());
    m_record.setValue("createtime", ui->dateEdit_create->date());
    m_record.setValue("expiration", ui->dateEdit_expire->date());
    m_record.setValue("company", ui->lineEdit_company->text());
    m_record.setValue("mobile", ui->lineEdit_tel->text());
    m_record.setValue("memo", ui->plainTextEdit->toPlainText());
    return m_record;
}
