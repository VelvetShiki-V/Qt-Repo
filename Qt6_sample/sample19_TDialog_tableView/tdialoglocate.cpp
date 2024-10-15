#include "tdialoglocate.h"
#include "ui_tdialoglocate.h"

TDialogLocate::TDialogLocate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogLocate)
{
    ui->setupUi(this);
}

TDialogLocate::~TDialogLocate()
{
    QMessageBox::information(this, "提示框", "会话框对象已删除");
    delete ui;
}

void TDialogLocate::set_spin_range(int row_count, int col_count)
{
    // 定位的行列号从0开始
    ui->spinBox_row->setRange(0, row_count - 1);
    ui->spinBox_column->setRange(0, col_count - 1);
}

void TDialogLocate::get_row_column(int row, int column)
{
    // 获取model的当前选中的单元格坐标值，并赋值给spinbox
    ui->spinBox_row->setValue(row);
    ui->spinBox_column->setValue(column);
}

void TDialogLocate::on_pushButton_apply_clicked()
{
    // 将文本框设置的值，以及行列增的单元格值以信号方式传出，并在槽函数中传输给mainwindow对应方法，以此赋值给model
    QString content = ui->lineEdit->text();
    int row = ui->spinBox_row->value();
    int column = ui->spinBox_column->value();
    // 当选择行增或列增功能时，将两边行列值同步并显示
    if(ui->checkBox_row->isChecked()) {
        ui->spinBox_row->setValue(++row);
    }
    if(ui->checkBox_column->isChecked()) {
        ui->spinBox_column->setValue(++column);
    }
    // 将应用更改后的行列值，以及文本值以信号形式发出传递给主窗体槽函数使用
    emit text_change(row, column, content);
}

// 关闭与打开对话框时，定位功能按钮的启动与禁止
void TDialogLocate::closeEvent(QCloseEvent *event)
{
    emit locate_button_enable(true);
    event->accept();
}

void TDialogLocate::showEvent(QShowEvent *event)
{
    emit locate_button_enable(false);
    event->accept();
}

