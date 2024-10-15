#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->dial, &QSlider::valueChanged, this, &Widget::do_slider_value_changed);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &Widget::do_slider_value_changed);
    connect(ui->horizontalScrollBar, &QSlider::valueChanged, this, &Widget::do_slider_value_changed);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::do_slider_value_changed(int value)
{
    ui->progressBar->setValue(value);
}


void Widget::on_checkBox_visible_clicked(bool checked)
{
    ui->progressBar->setTextVisible(checked);
}


void Widget::on_checkBox_inverted_clicked(bool checked)
{
    ui->progressBar->setInvertedAppearance(checked);
}


void Widget::on_radioButton_percent_clicked()
{
    ui->progressBar->setFormat("%p%");      // %p百分比格式化输出
}


void Widget::on_radioButton_value_clicked()
{
    ui->progressBar->setFormat("%v");      // %v当前值格式化输出
}

