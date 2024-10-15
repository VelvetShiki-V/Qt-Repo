#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QObject::connect(ui->radioButton_Red, SIGNAL(clicked()), this, SLOT(font_color()));
    QObject::connect(ui->radioButton_Green, SIGNAL(clicked()), this, SLOT(font_color()));
    QObject::connect(ui->radioButton_Blue, SIGNAL(clicked()), this, SLOT(font_color()));
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_checkBox_Underline_clicked(bool checked)
{
    QFont font = ui->plainTextEdit->font();
    font.setUnderline(checked);     // 获取当前输入框的文字信息
    ui->plainTextEdit->setFont(font);   // 将文字样式替换到GUI中
}


void Widget::on_checkBox_Italic_clicked(bool checked)
{
    QFont font = ui->plainTextEdit->font();
    font.setItalic(checked);
    ui->plainTextEdit->setFont(font);
}


void Widget::on_checkBox_Bold_clicked(bool checked)
{
    QFont font = ui->plainTextEdit->font();
    font.setBold(checked);
    ui->plainTextEdit->setFont(font);
}

void Widget::font_color()
{
    QPalette pal = ui->plainTextEdit->palette();
    if(ui->radioButton_Red->isChecked()) {
        pal.setColor(QPalette::Text, Qt::red);
    }
    else if(ui->radioButton_Green->isChecked()) {
        pal.setColor(QPalette::Text, Qt::green);
    }
    else if(ui->radioButton_Blue->isChecked()) {
        pal.setColor(QPalette::Text, Qt::blue);
    }
    ui->plainTextEdit->setPalette(pal);
}

