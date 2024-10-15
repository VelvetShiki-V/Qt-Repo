#include "tcomboboxdelegate.h"

TComboBoxDelegate::TComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

void TComboBoxDelegate::set_list_content(const QStringList &list, bool editable)
{
    m_content_list = list;
    m_editable = editable;
}

QWidget *TComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QComboBox* combo = new QComboBox(parent);
    combo->setFrame(false);
    combo->setEditable(m_editable);
    for(const auto& e: m_content_list) {
        combo->addItem(e);
    }
    return combo;
}

void TComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* combo = dynamic_cast<QComboBox*>(editor);
    combo->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
}

void TComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* combo = dynamic_cast<QComboBox*>(editor);
    model->setData(index, combo->currentText(), Qt::EditRole);
}

void TComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
