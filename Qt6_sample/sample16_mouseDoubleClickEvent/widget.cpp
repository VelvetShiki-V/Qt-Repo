#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 自定义label双击的信号与槽函数关联起来
    connect(ui->label, &TMyLabel::doubleClickSignal, this, &Widget::labelDoubleClicked);
}

Widget::~Widget()
{
    delete ui;
}

// label双击自定义事件
void Widget::labelDoubleClicked()
{
    ui->label->setText("label has been clicked twice!!!");
}

// 窗体双击事件
void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    ui->label->setText("widget has been clicked twice");
}
