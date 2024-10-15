#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
private:
    // 多选框控制字体样式
    QCheckBox *checkbox_bold;
    QCheckBox *checkbox_italic;
    QCheckBox *checkbox_underline;
    // 单选框控制字体颜色
    QRadioButton *radiobutton_red;
    QRadioButton *radiobutton_green;
    QRadioButton *radiobutton_blue;
    // 文本框
    QPlainTextEdit * txt;
    // 按钮
    QPushButton *pushbutton_submit;
    QPushButton *pushbutton_reset;
    QPushButton *pushbutton_cancel;
private slots:
    void set_txt_underline(bool checked);
    void set_txt_bold(bool checked);
    void set_txt_italic(bool checked);
    void set_txt_color();
};
#endif // DIALOG_H
