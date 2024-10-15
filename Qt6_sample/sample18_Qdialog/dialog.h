#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <QColor>
#include <QColorDialog>
#include <QFontDialog>
#include <QProgressDialog>
#include <QProgressBar>
#include <QElapsedTimer>
#include <windows.h>
#include <QInputDialog>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_open_file_clicked();

    void on_pushButton_open_files_clicked();

    void on_pushButton_select_dir_clicked();

    void on_pushButton_save_file_clicked();

    void on_pushButton_select_color_clicked();

    void on_pushButton_select_font_clicked();

    void on_pushButton_progress_clicked();

    void on_pushButton_input_str_clicked();

    void on_pushButton_input_int_clicked();

    void on_pushButton_input_float_clicked();

    void on_pushButton_combo_clicked();

    void on_pushButton_question_clicked();

    void on_pushButton_information_clicked();

    void on_pushButton_warning_clicked();

    void on_pushButton_critical_clicked();

    void on_pushButton_about_clicked();

    void on_pushButton_aboutQT_clicked();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
