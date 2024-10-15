#include "tdoublespinboxdelegate.h"

TDoubleSpinboxDelegate::TDoubleSpinboxDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QWidget *TDoubleSpinboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QSpinBox* spinbox = new QSpinBox(parent);
    spinbox->setFrame(false);
    spinbox->setRange(0, 3000);
    return spinbox;
}

void TDoubleSpinboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(editor);
    spinbox->setValue(index.model()->data(index, Qt::EditRole).toInt());
}

void TDoubleSpinboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox* spinbox = dynamic_cast<QSpinBox*>(editor);
    model->setData(index, spinbox->value(), Qt::EditRole);
}

void TDoubleSpinboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
