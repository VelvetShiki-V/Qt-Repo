#include "tdialogrbacdata.h"
#include "ui_tdialogrbacdata.h"

TDialogRbacData::TDialogRbacData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogRbacData)
{
    ui->setupUi(this);

}

TDialogRbacData::~TDialogRbacData()
{
    delete ui;
}

void TDialogRbacData::set_record(QSqlRelationalTableModel* model, int cur_row)
{
    // 下拉框空间初始化
    QSqlTableModel* relate = model->relationModel(foreign_role);
    for(int i = 0; i < relate->rowCount(); ++i) {
        ui->comboBox_name->addItem(relate->record(i).value("name").toString());
    }
    relate = model->relationModel(foreign_access);
    for(int i = 0; i < relate->rowCount(); ++i) {
        ui->comboBox_access->addItem(relate->record(i).value("name").toString());
    }
    relate = model->relationModel(foreign_domain);
    for(int i = 0; i < relate->rowCount(); ++i) {
        ui->comboBox_domain->addItem(relate->record(i).value("name").toString());
    }
    // 当前内容设置
    m_record = model->record(cur_row);
    ui->comboBox_name->setCurrentText(m_record.value(foreign_role_name).toString());
    ui->comboBox_access->setCurrentText(m_record.value(foreign_access_name).toString());
    ui->comboBox_domain->setCurrentText(m_record.value(foreign_domain_name).toString());
}

QSqlRecord TDialogRbacData::get_record()
{
//    qDebug() << "before set value:";
//    qDebug() << m_record.fieldName(0) << ":" << m_record.value(foreign_role_name);
//    qDebug() << m_record.fieldName(1) << ":" << m_record.value(foreign_access_name);
//    qDebug() << m_record.fieldName(2) << ":" << m_record.value(foreign_domain_name);

    m_record.setValue(foreign_role_name, ui->comboBox_name->currentText());
    m_record.setValue(foreign_access_name, ui->comboBox_access->currentText());
    m_record.setValue(foreign_domain_name, ui->comboBox_domain->currentText());

//    qDebug() << "after set value:";
//    qDebug() << m_record.fieldName(0) << ":" << m_record.value(foreign_role_name);
//    qDebug() << m_record.fieldName(1) << ":" << m_record.value(foreign_access_name);
//    qDebug() << m_record.fieldName(2) << ":" << m_record.value(foreign_domain_name);
    return m_record;
}
