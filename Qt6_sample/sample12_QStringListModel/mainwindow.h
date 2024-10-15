#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QListView>
#include <QModelIndex>
#include <QStringListModel>

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

    void on_pushButton_restore_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_append_clicked();

    void on_pushButton_insert_clicked();

    void on_checkBox_clicked(bool checked);

    void on_pushButton_9_clicked();

    void on_pushButton_sort_clicked(bool checked);

    void on_pushButton_move_up_clicked();

    void on_pushButton_move_down_clicked();

    void on_pushButton_clear_txt_clicked();

    void on_pushButton_display_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    QStringList m_list;         // 用于备份存储初始化的stringlist
    QStringListModel* m_model;  // 所有按钮控件都根据此模型进行控制

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
