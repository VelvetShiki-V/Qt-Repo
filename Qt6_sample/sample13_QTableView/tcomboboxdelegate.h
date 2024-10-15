#ifndef TCOMBOBOXDELEGATE_H
#define TCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QObject>

class TComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TComboBoxDelegate(QObject *parent = nullptr);
    void set_list_content(const QStringList& list, bool editable);

private:
    // 额外的属性，以用来初始化下拉框内容
    QStringList m_content_list;
    bool m_editable;        // 允许是否编辑下拉框

    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TCOMBOBOXDELEGATE_H
