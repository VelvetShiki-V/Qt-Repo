#include "tcomboxdelegate.h"

void TComboxDelegate::set_items(const QStringList &items, bool editable)
{
    m_list = items;
    m_editable = editable;
}

TComboxDelegate::TComboxDelegate(QObject* parent)
    :QStyledItemDelegate{parent}
{}


QWidget *TComboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox* combo = new QComboBox(parent);
    combo->setFrame(false);
    combo->setEditable(m_editable);
    for(const auto& e: m_list) {
        combo->addItem(e);
    }
    return combo;
}

void TComboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* combo = dynamic_cast<QComboBox*>(editor);
    combo->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
}

void TComboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* combo = dynamic_cast<QComboBox*>(editor);
    model->setData(index, combo->currentText(), Qt::EditRole);
}

void TComboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
