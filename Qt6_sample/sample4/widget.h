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
    void on_pushButton_clean_clicked();

    void on_pushButton_front_clicked();

    void on_pushButton_left_clicked();

    void on_pushButton_first_clicked();

    void on_pushButton_section_clicked();

    void on_pushButton_null_clicked();

    void on_pushButton_resize_clicked();

    void on_pushButton_size_clicked();

    void on_pushButton_trimmed_clicked();

    void on_pushButton_insert_clicked();

    void on_pushButton_remove_clicked();

    void on_pushButton_replace_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
