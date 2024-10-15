#ifndef TDIALOGEMPLOYEEDATA_H
#define TDIALOGEMPLOYEEDATA_H

#include <QDialog>
#include <QSqlRecord>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>

namespace Ui {
class TDialogEmployeeData;
}

class TDialogEmployeeData : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogEmployeeData(QWidget *parent = nullptr);
    ~TDialogEmployeeData();

    void set_record(const QSqlRecord& record);
    QSqlRecord get_record();

private slots:
    void on_pushButton_photo_input_clicked();

    void on_pushButton_photo_clear_clicked();

private:
    Ui::TDialogEmployeeData *ui;
    QSqlRecord m_record;
    QStringList m_gender;
    QStringList m_dept;
    QStringList m_province;
};

#endif // TDIALOGEMPLOYEEDATA_H
