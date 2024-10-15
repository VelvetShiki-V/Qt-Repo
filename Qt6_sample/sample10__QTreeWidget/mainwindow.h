#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QFileDialog>
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QPixmap>
#include <QLabel>
#include <QStatusBar>
#include <QSpinBox>

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
    void on_action_add_dir_triggered();

    void on_action_add_file_triggered();

    void on_action_delete_node_triggered();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_action_node_traversal_triggered();

    void on_action_nice_height_triggered();

    void on_dockWidget_visibilityChanged(bool visible);

    void on_dockWidget_topLevelChanged(bool topLevel);

    void on_action_float_window_triggered(bool checked);

    void on_action_window_visible_triggered(bool checked);

    void on_action_zoom_out_triggered();

    void on_action_nice_weight_triggered();

    void on_action_restore_triggered();

    void on_action_zoom_in_triggered();

private:
    // 图片属性
    QPixmap m_pic;          // 图片流
    float m_ratio = 1;      // 默认缩放比例
    QSpinBox* ratio_info;   // 状态栏显示（动态调整）
    QLabel* pic_path;       // 图片路径（动态调整）
    QLabel* status_info;    // 图片标题4信息显示

    enum {Titem = 0, Ttype, Tdate};
    enum {Troot = 1000, Tfolder, Tfile};        // 用户定义文件类型从1000号开始，前1000为Qt内核使用
    void caption_traversal(QTreeWidgetItem* item);
    void image_display(QTreeWidgetItem* item);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
