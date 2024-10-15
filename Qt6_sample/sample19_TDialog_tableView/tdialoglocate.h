#ifndef TDIALOGLOCATE_H
#define TDIALOGLOCATE_H

#include <QDialog>
#include <QMessageBox>
#include <QShowEvent>
#include <QCloseEvent>

namespace Ui {
class TDialogLocate;
}

class TDialogLocate : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogLocate(QWidget *parent = nullptr);
    ~TDialogLocate();

public:
    // 设置spinbox范围
    void set_spin_range(int row_count, int col_count);
    // 点击对应单元格同步显示到spinbox（非模态同步显示）
    void get_row_column(int rows, int columns);


private slots:
    void on_pushButton_apply_clicked();

signals:
    // 行增列增，或文本更改后，信号传递到model触发槽函数的内容
    void text_change(int row, int column, QString content);
    // 信号控制是否允许按钮可用
    void locate_button_enable(bool flag);
private:
    Ui::TDialogLocate *ui;

    // 当对话框打开和关闭状态时触发的事件（定位按钮启用）
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
};



#endif // TDIALOGLOCATE_H
