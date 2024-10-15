#ifndef TDIALOGROLEDATA_H
#define TDIALOGROLEDATA_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
class TDialogRoleData;
}

class TDialogRoleData : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogRoleData(QWidget *parent = nullptr);
    ~TDialogRoleData();

    void set_record(const QSqlRecord& record);
    QSqlRecord get_record();

private:
    Ui::TDialogRoleData *ui;
    QSqlRecord m_record;
};

#endif // TDIALOGROLEDATA_H
