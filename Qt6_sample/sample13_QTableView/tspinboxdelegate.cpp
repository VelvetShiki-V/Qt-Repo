#include "tspinboxdelegate.h"

// 旋钮单元格类，参数与继承关系与基类保持一致
TSpinboxDelegate::TSpinboxDelegate(QObject* parent): QStyledItemDelegate(parent)
{}

QWidget *TSpinboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QSpinBox* spinbox = new QSpinBox(parent);
    spinbox->setFrame(false);
    spinbox->setRange(0, 2000);
    return spinbox;
}

void TSpinboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // 代理组件传入需要强转，并将模型数据同步
    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(editor);
    int val = index.model()->data(index, Qt::EditRole).toInt();
    spinbox->setValue(val);
}

void TSpinboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(editor);
    model->setData(index, spinbox->value(), Qt::EditRole);
}

void TSpinboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
