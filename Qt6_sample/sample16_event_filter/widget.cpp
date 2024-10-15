#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 为需要过滤的组件label注册filter，并加入对象树管理
    ui->label_SC->installEventFilter(this);
    ui->label_DC->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}


bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    // 监视事件为单击
    if(watched == ui->label_SC) {
        // 鼠标悬浮经过监视区域label
        if(event->type() == QEvent::Enter)
        {
            ui->label_SC->setStyleSheet("background-color: rgb(248,128,128)");
        }
        // 鼠标悬浮离开监视区域label
        else if(event->type() == QEvent::Leave) {
            ui->label_SC->setText("Press this label once");
            ui->label_SC->setStyleSheet("");
        }
        // 鼠标单击和松开过滤事件
        else if(event->type() == QEvent::MouseButtonPress)
            ui->label_SC->setText("Label has been pressed");
        else if(event->type() == QEvent::MouseButtonRelease)
            ui->label_SC->setText("Label has been released");
    }

    // 监视事件为双击
    if(watched == ui->label_DC) {
        if(event->type() == QEvent::Enter)
            ui->label_DC->setStyleSheet("background-color: rgb(29,84,92)");
        else if(event->type() == QEvent::Leave) {
            ui->label_DC->setText("Double click this label");
            ui->label_DC->setStyleSheet("");
        }
        else if(event->type() == QEvent::MouseButtonDblClick)
            ui->label_DC->setText("Label has been double clicked");
    }
    // 基类初始化（必要）
    return QWidget::eventFilter(watched, event);
}
