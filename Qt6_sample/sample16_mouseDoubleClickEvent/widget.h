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

private slots:
    // label双击的自定义事件（需要由提升的myLabel类信号来触发）
    void labelDoubleClicked();

protected:
    // 窗体双击触发的事件（原生事件，仅能应用于窗体本身，而不能自定义组件双击行为）
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};


#endif // WIDGET_H
