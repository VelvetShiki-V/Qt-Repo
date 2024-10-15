#ifndef TDIALOGTABLEVIEW_H
#define TDIALOGTABLEVIEW_H

#include <QDialog>
#include <QSpinBox>
#include <QMessageBox>

namespace Ui {
class TDialogTableView;
}

class TDialogTableView : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogTableView(QWidget *parent = nullptr);
    ~TDialogTableView();

public:
    // 从当前模型获取行列信息
    void set_column_rows(int columns, int rows);
    // 将从对话框更改的行列数据同步到tableView模型视图中
    int get_column_changed();
    int get_row_changed();

private:
    Ui::TDialogTableView *ui;
};

#endif // TDIALOGTABLEVIEW_H
