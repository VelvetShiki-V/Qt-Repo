#ifndef TDIALOGDOMAINDATA_H
#define TDIALOGDOMAINDATA_H

#include <QDialog>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>

namespace Ui {
class TDialogDomainData;
}

class TDialogDomainData : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogDomainData(QWidget *parent = nullptr);
    ~TDialogDomainData();

    void set_record(QSqlRelationalTableModel* model, int cur_row);
    QSqlRecord get_record();

private:
    const int foreign_super_column = 1;
    QString foreign_super_name = "domain_name_2";

private:
    Ui::TDialogDomainData *ui;
    QSqlRecord m_record;
};

#endif // TDIALOGDOMAINDATA_H
