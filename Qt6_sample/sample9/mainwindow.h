#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QMenu>

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
    void on_action_initializer_triggered();

    void on_action_clear_triggered();

    void on_action_insert_triggered();

    void on_action_add_triggered();

    void on_action_delete_triggered();

    void on_action_all_triggered();

    void on_action_none_triggered();

    void on_action_inv_triggered();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_toolButton_txt_clear_clicked();

    void on_toolButton_empty_line_clicked();

    void on_checkBox_sort_clicked(bool checked);

    void on_toolButton_ascend_clicked();

    void on_toolButton_descend_clicked();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
