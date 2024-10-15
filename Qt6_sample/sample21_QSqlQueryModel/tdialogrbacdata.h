#ifndef TDIALOGRBACDATA_H
#define TDIALOGRBACDATA_H

#include <QDialog>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

namespace Ui {
class TDialogRbacData;
}

class TDialogRbacData : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogRbacData(QWidget *parent = nullptr);
    ~TDialogRbacData();

public:
    void set_record(QSqlRelationalTableModel* model, int cur_row);
    QSqlRecord get_record();
private:
    QSqlRecord m_record;
    enum {foreign_role = 0, foreign_access, foreign_domain};
    // 外键在relation模型中会被特殊处理成为不会冲突的新列字段名，需要根据编码结果进行特殊转换
    const QString foreign_role_name = "role_name_3";
    const QString foreign_access_name = "access_name_2";
    const QString foreign_domain_name = "name";

private:
    Ui::TDialogRbacData *ui;
};

#endif // TDIALOGRBACDATA_H
