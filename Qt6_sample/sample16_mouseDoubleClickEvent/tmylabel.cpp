#include "tmylabel.h"

TMyLabel::TMyLabel(QWidget* parent)
    :QLabel(parent)
{
    // 设置窗体属性并启用悬浮功能
    setAttribute(Qt::WA_Hover, true);
}

// 双击事件的实际用途是发射信号并与connect中定义的自定义槽函数关联
void TMyLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit doubleClickSignal();
}

bool TMyLabel::event(QEvent *event)
{
    // 基类需要初始化（否则将不会产生效果）
    QLabel::event(event);
    // hoverenter为鼠标悬浮，hoverleave为鼠标移开，两个事件配成一对，缺一不可
    if(event->type() == QEvent::HoverEnter) {
        QPalette pal = this->palette();                       // 由label触发
        pal.setColor(QPalette::WindowText, Qt::red);        // 必须使用setColor，因为要对windowText设置颜色
        setPalette(pal);
    }
    else if(event->type() == QEvent::HoverLeave) {
        QPalette pal = this->palette();
        pal.setColor(QPalette::WindowText, Qt::black);
        setPalette(pal);
    }
    return true;
}
