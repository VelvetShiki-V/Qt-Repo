#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QString>
#include <QRandomGenerator>
#include <QDate>
#include <QLabel>
#include <QStatusBar>
#include <QAbstractButton>

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
    void on_pushButton_header_clicked();

    void on_pushButton_row_set_clicked();

    void on_pushButton_init_clicked();

    void on_pushButton_row_insert_clicked();

    void on_pushButton_row_append_clicked();

    void on_pushButton_row_delete_clicked();

    void on_pushButton_row_adjust_clicked();

    void on_pushButton_column_adjust_clicked();

    void on_pushButton_read_clicked();

    void on_checkBox_editable_clicked(bool checked);

    void on_checkBox_color_clicked(bool checked);

    void on_checkBox_header_clicked(bool checked);

    void on_checkBox_vHeader_clicked(bool checked);

    void on_radioButton_row_select_clicked();

    void on_radioButton_item_select_clicked();

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    // 每个表格单元格的属性（用于状态栏显示）
    QLabel* cell_pos;           // 单元格坐标
    QLabel* cell_type;          // 单元格类型（用户自定义）
    QLabel* student_id;         // 学生学号
    enum {colName = 0, colSex, colDate, colEthnic, colGrade, colPM};        // 控制表格列
    enum {CTname = 1000, CTsex, CTdate, CTethnic, CTgrade, CTpm};           // 控制表格类型

    // 单行信息插入
    void info_registration_to_one_row(int row, const QString& name, const QString& sex,
                                      const QDate& date, const QString& ethnic,
                                      const short& grade, bool pm);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
