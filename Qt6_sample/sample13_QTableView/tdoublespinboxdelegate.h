#ifndef TDOUBLESPINBOXDELEGATE_H
#define TDOUBLESPINBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QObject>

class TDoubleSpinboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TDoubleSpinboxDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TDOUBLESPINBOXDELEGATE_H
