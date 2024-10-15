#ifndef TDIALOGACCESSDATA_H
#define TDIALOGACCESSDATA_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
class TDialogAccessData;
}

class TDialogAccessData : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogAccessData(QWidget *parent = nullptr);
    ~TDialogAccessData();

    void set_record(const QSqlRecord& record);
    QSqlRecord get_record();

private:
    Ui::TDialogAccessData *ui;
    QSqlRecord m_record;
};

#endif // TDIALOGACCESSDATA_H
