#ifndef TDIALOGNEWPROJECT_H
#define TDIALOGNEWPROJECT_H

#include <QDialog>

namespace Ui {
class TDialogNewProject;
}

class TDialogNewProject : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogNewProject(QWidget *parent = nullptr);
    ~TDialogNewProject();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::TDialogNewProject *ui;

signals:
    void project_name_apply(const QString& name);
};

#endif // TDIALOGNEWPROJECT_H
