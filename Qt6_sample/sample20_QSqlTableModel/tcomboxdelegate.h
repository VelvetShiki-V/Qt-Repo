#ifndef TCOMBOXDELEGATE_H
#define TCOMBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QComboBox>

class TComboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    // 对外接口，可以将录好的内容存入其中，以供用户下拉选择
    void set_items(const QStringList& items, bool editable);
    explicit TComboxDelegate(QObject* parent = nullptr);

private:
    // 提前存储内容信息，以及是否可编辑
    QStringList m_list;
    bool m_editable;

    // 针对性别和部门设置的单元格下拉框样式功能
public:
    //
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TCOMBOXDELEGATE_H
