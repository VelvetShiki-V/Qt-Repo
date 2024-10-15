#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFileSystemModel>

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
    // model控制三个视图：treeView, tableView, listView
    QFileSystemModel* m_model;

private slots:
    void on_action_root_dir_triggered();

    void on_radioButton_only_dir_clicked();

    void on_radioButton_all_clicked();

    void on_checkBox_filter_clicked(bool checked);

    void on_pushButton_apply_clicked();

    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
