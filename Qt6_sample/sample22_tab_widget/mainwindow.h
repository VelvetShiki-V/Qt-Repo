#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

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
    // 嵌入widget
    void on_action_widget_insert_triggered();

    // 独立widget
    void on_action_widget_indep_triggered();

    // 嵌入widget标题更改
    void change_title(const QString& title);

    // widget关闭按钮事件
    void on_tabWidget_tabCloseRequested(int index);

    // 嵌入mainWindow
    void on_action_mainwindow_insert_triggered();

    // 独立mainWindow
    void on_action_mainwindow_indep_triggered();

    void on_action_new_widget_triggered();

    void on_action_open_widget_triggered();

    void on_action_font_triggered();

    void on_action_cut_triggered();

    void on_action_copy_triggered();

    void on_action_paste_triggered();

    void on_action_cascade_triggered();

    void on_action_tile_triggered();

    void on_action_close_all_triggered();

    void content_selected();

    void content_changed();

    void on_action_save_triggered();

    void on_actionMDI_triggered(bool checked);

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
};


#endif // MAINWINDOW_H
