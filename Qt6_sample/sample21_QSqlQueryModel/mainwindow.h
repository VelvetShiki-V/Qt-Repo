#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QTreeView>
#include <QLabel>
#include <QSql>
#include <QMenu>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>
#include <QFileDialog>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QStandardItemModel>
#include <QShowEvent>
#include <QCryptographicHash>

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
    void on_tableView_clicked(const QModelIndex& index);

    void on_treeView_clicked(const QModelIndex &index);

    void on_action_append_triggered();

    void on_action_edit_triggered();

    void on_action_delete_triggered();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_action_connect_triggered();

    void on_checkBox_specific_clicked(bool checked);

    void on_tableView_customContextMenuRequested(const QPoint &pos);

    void on_tableItem_move(const QModelIndex& index);

    void on_action_next_triggered();

    void on_action_previous_triggered();

    void on_action_first_triggered();

    void on_action_last_triggered();

private:
    QSqlDatabase DB;
//    FileNameFileSystemModel* file_model;      // 用于本地文件系统搜索
    // 根据数据库选用的模型，分为query模型（用于表的CRUD操作）,relation模型（外键关联表CRUD操作），selection来选择表格单元格
    QItemSelectionModel* m_selection = nullptr;
    QSqlQueryModel* sql_model = nullptr;
    QStandardItemModel* database_model = nullptr;     // standardItemMode用于控制treeView中的父子项
    QStandardItem* table_list = nullptr;              // 用于在treeView中作为子项依附的父项
    QSqlRelationalTableModel* relation_model = nullptr;
    QLabel* database_name = nullptr;           // 状态栏标签，分别为数据库名，表名，行定位，记录数
    QLabel* table_name = nullptr;
    QLabel* row_locate = nullptr;
    QLabel* record_count = nullptr;
    QString current_table_name;     // 打开的当前表格名
    bool first_time_show = true;   // 首次登录将弹出登录对话框
    bool is_filtered = true;       // 是否过滤数据库和表
    const int current_row = -1;
    // 设置初始化数据库时打开的数据库和表名
    const QString default_open_database = "myemc";
    const QString default_open_table = "user";
    // 设置treeView是否需要过滤数据库或表，以及过滤的名称
    const QStringList db_filter = (QStringList() << "myemc");
    const QStringList table_filter = (QStringList() << "role" << "access" << "domain" << "rbac" << "user");
    // 区分主表和关联表，用于表在tableView中的显示和表数据的修改处理
    enum {query_model = 1, relational_model};

private:
    bool user_authenticate();
    void list_all_databases();
    void list_tables(const QModelIndex &index);
    void select_data();
    void header_handle();
    bool is_relational();
    void show_relational_table();
    void set_relation();
    void newest_locate(QAbstractTableModel* model, const QSqlRecord& record, const int& model_flag = 1);
    void select_row(const int& row);
    void memory_leak_check_release();
    QString calculate_sha256(const QString& password);

private:
    Ui::MainWindow *ui;

    // QWidget interface打开界面时显示的弹出对话框
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // MAINWINDOW_H
