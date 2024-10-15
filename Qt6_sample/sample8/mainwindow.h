#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

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
    QLabel *status = new QLabel(this);


private slots:
    void on_action_new_file_triggered();

    void on_action_open_file_triggered();

    void on_action_save_file_triggered();

    void on_textEdit_copyAvailable(bool b);

    void on_textEdit_selectionChanged();

    void on_action_bold_triggered(bool checked);

    void on_action_italic_triggered(bool checked);

    void on_action_underline_triggered(bool checked);

    void on_action_spinbox_fontsizeChanged(int size);

    void on_action_combobox_fontstyleChanged(QFont style);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
