#include "tdialogemployeedata.h"
#include "ui_tdialogemployeedata.h"

TDialogEmployeeData::TDialogEmployeeData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogEmployeeData)
{
    // 将对话框初始控件值填充到界面上以供用户选择
    ui->setupUi(this);
    ui->spinBox_empNum->setRange(10000, 99999);
    ui->spinBox_salary->setRange(1000, 99999);
    m_gender << "男" << "女";
    m_dept << "研发部" << "销售部" << "运维部" << "宣传部" << "对策部";
    m_province << "上海" << "北京" << "广州" << "深圳" << "云南";
    ui->comboBox_dept->addItems(m_dept);
    ui->comboBox_gender->addItems(m_gender);
    ui->comboBox_province->addItems(m_province);
}

TDialogEmployeeData::~TDialogEmployeeData()
{
    delete ui;
}

// 将model的表数据格式和值同步给对话框（初始化必要）
void TDialogEmployeeData::set_record(const QSqlRecord &record)
{
    m_record = record;
    ui->spinBox_empNum->setValue(record.value("empNum").toInt());
    ui->plainTextEdit_name->appendPlainText(record.value("empName").toString());
    ui->comboBox_gender->setCurrentText(record.value("gender").toString());
    ui->dateEdit->setDate(record.value("birth").toDate());
    ui->comboBox_province->setCurrentText(record.value("province").toString());
    ui->comboBox_dept->setCurrentText(record.value("empDept").toString());
    ui->spinBox_salary->setValue(record.value("salary").toInt());
    ui->plainTextEdit->appendPlainText(record.value("memo").toString());

    // 图片输出（若没有则不初始化图片）
    QVariant var = record.value("photo");
    if(!var.isValid()) {
        ui->label_pic->clear();
        QMessageBox::warning(this, "警告", "图片加载失败");
        return;
    }
    QPixmap pic;
    pic.loadFromData(var.toByteArray());
    ui->label_pic->setPixmap(pic.scaledToWidth(ui->label_pic->size().width()));
}

// 将对话框中的值同步给模型tableView
QSqlRecord TDialogEmployeeData::get_record()
{
    // 字段值的传入和传出必须与模型，数据库字段名一致（否则可能将同步失败）
    m_record.setValue("empNum", ui->spinBox_empNum->value());
    m_record.setValue("empName", ui->plainTextEdit_name->toPlainText());
    m_record.setValue("gender", ui->comboBox_gender->currentText());
    m_record.setValue("birth", ui->dateEdit->date());
    m_record.setValue("province", ui->comboBox_province->currentText());
    m_record.setValue("empDept", ui->comboBox_dept->currentText());
    m_record.setValue("salary", ui->spinBox_salary->value());
    m_record.setValue("memo", ui->plainTextEdit->toPlainText());
    return m_record;
}

// 导入图片按钮，加载图片流并将图片通过pixmap和label输出到界面框
void TDialogEmployeeData::on_pushButton_photo_input_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, "选择图片", "", "图片文件(*.png *.jpg)");
    if(file_path.isEmpty()) return;
    QFile pic_file(file_path);
    if(!pic_file.open(QIODevice::ReadOnly)) return;
    QByteArray pic_stream = pic_file.readAll();
    pic_file.close();
    QPixmap pic;
    pic.loadFromData(pic_stream);
    m_record.setValue("photo", pic_stream);
    ui->label_pic->setPixmap(pic.scaledToWidth(ui->label_pic->size().width()));
}


void TDialogEmployeeData::on_pushButton_photo_clear_clicked()
{
    m_record.setNull("photo");
    ui->label_pic->clear();
}

