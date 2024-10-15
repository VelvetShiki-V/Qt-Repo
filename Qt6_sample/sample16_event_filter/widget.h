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

private:
    Ui::Widget *ui;

    // 基类重写事件过滤器方法，作用于label，当lable被鼠标滑过，单击或双击产生如同mouseClick事件的类的效果
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // WIDGET_H
