#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "tdialogtableview.h"
#include "tdialogheaderlist.h"
#include "tdialoglocate.h"
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_set_row_column_triggered();

    void on_action_set_header_triggered();

    void on_action_locate_triggered();

    // 对话框修改文本的信号发出，主窗口槽函数方法被执行并赋值修改对应单元格内容
    void set_text(int row, int column, QString content);

    // 当点击单元格时，对话框spinbox坐标被改变
    void on_tableView_clicked(const QModelIndex &index);

    // 当当前单元格位置发生变化，由QItemSelectionModel触发的currentChanged信号对应的槽函数方法被执行
    void current_item_changed(const QModelIndex &current, const QModelIndex &previous);


signals:
    // 对应槽函数方法为on_tableView_clicked，以此信号来传递行列信息给对话框
    void tell_current_row_column(int row, int column);

private:
    QStandardItemModel* m_model;
    QItemSelectionModel* m_selection;
    TDialogHeaderList* dialog_header;       // 可以选择统一管理到对象树上，也可以在局部创建使用和删除

    QLabel* cell_pos;
    QLabel* cell_content;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
