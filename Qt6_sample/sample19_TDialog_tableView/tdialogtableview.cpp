#include "tdialogtableview.h"
#include "ui_tdialogtableview.h"

TDialogTableView::TDialogTableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogTableView)
{
    ui->setupUi(this);
}

TDialogTableView::~TDialogTableView()
{
    QMessageBox::information(this, "消息框", "tableview对话框已删除");
    delete ui;
}

// 从tableView模型获取表格行列值，与其同步
void TDialogTableView::set_column_rows(int columns, int rows)
{
    ui->spinBox_column->setValue(columns);
    ui->spinBox_row->setValue(rows);
}

// 将修改的值同步给tableView模型，保持双边一致
int TDialogTableView::get_column_changed()
{
    return ui->spinBox_column->value();
}

int TDialogTableView::get_row_changed()
{
    return ui->spinBox_row->value();
}
