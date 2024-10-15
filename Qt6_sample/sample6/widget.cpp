#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_left_clicked()
{
    ui->lineEdit->setAlignment(Qt::AlignLeft);
}


void Widget::on_pushButton_center_clicked()
{
    ui->lineEdit->setAlignment(Qt::AlignCenter);
}


void Widget::on_pushButton_right_clicked()
{
    ui->lineEdit->setAlignment(Qt::AlignRight);
}

void Widget::on_pushButton_italic_clicked(bool checked)
{
    QFont font = ui->lineEdit->font();
    font.setItalic(checked);
    ui->lineEdit->setFont(font);
}


void Widget::on_pushButton_bold_clicked(bool checked)
{
    QFont font = ui->lineEdit->font();
    font.setBold(checked);
    ui->lineEdit->setFont(font);
}


void Widget::on_pushButton_underline_clicked(bool checked)
{
    QFont font = ui->lineEdit->font();
    font.setUnderline(checked);
    ui->lineEdit->setFont(font);
}


void Widget::on_checkBox_read_clicked(bool checked)
{
    ui->lineEdit->setReadOnly(checked);
}


void Widget::on_checkBox_enabled_clicked(bool checked)
{
    ui->lineEdit->setEnabled(checked);
}

void Widget::on_radioButton_red_clicked()
{
    QPalette pal = ui->lineEdit->palette();
    pal.setColor(QPalette::Text, Qt::red);
    ui->lineEdit->setPalette(pal);
}


void Widget::on_checkBox_clean_clicked()
{
    ui->lineEdit->clear();
}


void Widget::on_radioButton_green_clicked()
{
    QPalette pal = ui->lineEdit->palette();
    pal.setColor(QPalette::Text, Qt::green);
    ui->lineEdit->setPalette(pal);
}


void Widget::on_radioButton_blue_clicked()
{
    QPalette pal = ui->lineEdit->palette();
    pal.setColor(QPalette::Text, Qt::blue);
    ui->lineEdit->setPalette(pal);
}

