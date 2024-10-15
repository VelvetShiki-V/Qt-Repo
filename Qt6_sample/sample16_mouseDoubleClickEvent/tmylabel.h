#ifndef TMYLABEL_H
#define TMYLABEL_H

#include <QLabel>
#include <QObject>
#include <QEvent>
#include <QPalette>

// 由Qlabel提升来的自定义双击类，该类的作用在于封装单独的label双击信号，并触发关联重写的组件双击事件
class TMyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TMyLabel(QWidget* parent = nullptr);

signals:
    // 专门用于针对标签双击的信号（不需要实现，只是一个信号）
    void doubleClickSignal();

protected:
    // 双击标签发射信号
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    // QObject interface
public:
    // 鼠标悬停事件
    virtual bool event(QEvent *event) override;
};

#endif // TMYLABEL_H
