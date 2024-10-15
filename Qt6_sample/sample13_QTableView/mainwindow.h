#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QLabel>
#include <QTableView>
#include <QModelIndex>
#include <QFileDialog>
#include <QActionGroup>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QRegularExpression>

#include "tcomboboxdelegate.h"
#include "tdoublespinboxdelegate.h"
#include "tspinboxdelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // 状态栏显示的标签内容
    QLabel* cell_pos;
    QLabel* cell_content;
    QLabel* file_name;

    // model控制tableview，selection控制指针当前选中的单元格或当前行
    QStandardItemModel* m_model;
    QItemSelectionModel* m_selection;

    // 固定的表格列数常量
    const int column_num = 6;

    // 表格代理
    TSpinboxDelegate* spin_dlg;                 // 普通数值单元格代理
    TDoubleSpinboxDelegate* dspin_dlg;          // 浮点数值代理
    TComboBoxDelegate* combo_dlg;               // 下拉框代理

private:
    // 用于打开的文本给tableview数据初始化
    void modelInit(const QStringList& file_contents);

private slots:
    void do_currentItemChanged(const QModelIndex& current, const QModelIndex& previous);

    void on_action_open_file_triggered();

    void on_action_data_view_triggered();

    void on_action_add_row_triggered();

    void on_action_insert_row_triggered();

    void on_action_delete_row_triggered();

    void on_action_left_triggered();

    void on_action_center_triggered();

    void on_action_right_triggered();

    void on_action_bold_triggered(bool checked);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
