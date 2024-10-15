#ifndef TSPINBOXDELEGATE_H
#define TSPINBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QSpinBox>

class TSpinboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    // 继承基类写法，禁止隐式类型转换，需要挂载到对象树上
    explicit TSpinboxDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    // 设置单元格代理初始化（创建代理对象，设置基本属性）
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // 将模型数据与代理同步
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    // 将代理数据与模型同步
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    // 设置代理格式
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TSPINBOXDELEGATE_H
