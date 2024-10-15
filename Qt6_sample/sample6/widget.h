#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_left_clicked();

    void on_pushButton_center_clicked();

    void on_pushButton_right_clicked();

    void on_pushButton_italic_clicked(bool checked);

    void on_pushButton_bold_clicked(bool checked);

    void on_pushButton_underline_clicked(bool checked);

    void on_checkBox_read_clicked(bool checked);

    void on_checkBox_enabled_clicked(bool checked);





    void on_radioButton_red_clicked();

    void on_checkBox_clean_clicked();

    void on_radioButton_green_clicked();

    void on_radioButton_blue_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
