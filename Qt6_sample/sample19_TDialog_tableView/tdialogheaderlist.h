#ifndef TDIALOGHEADERLIST_H
#define TDIALOGHEADERLIST_H

#include <QDialog>
#include <QStringListModel>
#include <QMessageBox>

namespace Ui {
class TDialogHeaderList;
}

class TDialogHeaderList : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogHeaderList(QWidget *parent = nullptr);
    ~TDialogHeaderList();

public:
    // 从tableView模型获取现有的表头list
    void get_current_header_list(const QStringList& header);
    // 将从对话框更改的表头list同步到tableView模型上
    QStringList set_changed_list();
private:
    // list_model模型
    QStringListModel* m_model;
private:
    Ui::TDialogHeaderList *ui;
};

#endif // TDIALOGHEADERLIST_H
