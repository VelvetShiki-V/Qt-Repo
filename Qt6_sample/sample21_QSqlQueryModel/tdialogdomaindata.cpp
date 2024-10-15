#include "tdialogdomaindata.h"
#include "ui_tdialogdomaindata.h"

TDialogDomainData::TDialogDomainData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogDomainData)
{
    ui->setupUi(this);
}

TDialogDomainData::~TDialogDomainData()
{
    delete ui;
}

void TDialogDomainData::set_record(QSqlRelationalTableModel* model, int cur_row)
{
    QSqlTableModel* relate = model->relationModel(foreign_super_column);
    // 设置下拉框外键关联的主表字段显示
    for(int i = 0; i < relate->rowCount(); ++i) {
        ui->comboBox_super->addItem(relate->record(i).value("name").toString());
    }
    // 设置初始化对话框界面显示内容
    m_record = model->record(cur_row);
    ui->lineEdit->setText(m_record.value("name").toString());
    ui->comboBox_super->setCurrentText(m_record.value(foreign_super_name).toString());
    ui->plainTextEdit->appendPlainText(m_record.value("memo").toString());
}

QSqlRecord TDialogDomainData::get_record()
{
    m_record.setValue("name", ui->lineEdit->text());
    m_record.setValue(foreign_super_name, ui->comboBox_super->currentText());
    m_record.setValue("memo", ui->plainTextEdit->toPlainText());
    return m_record;
}
