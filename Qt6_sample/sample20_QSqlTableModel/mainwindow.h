#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSql>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QPixmap>
#include "tcomboxdelegate.h"

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
    // 定义打开数据库的功能
    void on_action_open_file_triggered();
    // 选择不同单元格时，对应按钮功能打开
    void do_current_item_changed(const QModelIndex& current, const QModelIndex& previous);
    // 光标移动到不同行时，按钮打开并渲染图片
    void do_current_row_changed(const QModelIndex& current, const QModelIndex& previous);

    void on_action_append_line_triggered();

    void on_action_insert_line_triggered();

    void on_action_delete_line_triggered();

    void on_action_photo_set_triggered();

    void on_action_photo_clear_triggered();

    void on_action_salary_increase_triggered();

    void on_action_save_triggered();

    void on_action_undo_triggered();

    void on_comboBox_order_currentIndexChanged(int index);

    void on_radioButton_ascend_clicked();

    void on_radioButton_descend_clicked();

    void on_radioButton_man_clicked();

    void on_radioButton_girl_clicked();

    void on_radioButton_all_sex_clicked();

private:
    // 存储打开用的数据库DB，渲染表格输出的model和选择selection
    QSqlDatabase DB;
    QSqlTableModel* m_model;
    QItemSelectionModel* m_selection;
    TComboxDelegate* delegate_gender;       // 自定义功能单元格代理框
    TComboxDelegate* delegate_dept;
    QDataWidgetMapper* m_mapper;            // 数据映射记录的mapper
    // 用于在打开数据库通道后，挑选其中一个表格进行输出
    void open_table();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
