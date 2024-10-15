#ifndef TDIALOGUSERDATA_H
#define TDIALOGUSERDATA_H

#include <QDialog>
#include <QSqlRecord>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

namespace Ui {
class TDialogUserData;
}

class TDialogUserData : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogUserData(QWidget *parent = nullptr);
    ~TDialogUserData();

public:
    void set_record(const QSqlRelationalTableModel* model, const int& cur_row);
    QSqlRecord get_record();
private:
    QSqlRecord m_record;
    // 为relation模型的外键进行的特殊处理
    const int foreign_role_column = 2;
    QString foreign_role_name = "role_name_2";

private:
    Ui::TDialogUserData *ui;
};

#endif // TDIALOGUSERDATA_H
