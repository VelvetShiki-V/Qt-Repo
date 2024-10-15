#ifndef TFORMWIDGET_H
#define TFORMWIDGET_H

#include <QWidget>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QMdiSubWindow>

namespace Ui {
class TFormWidget;
}

class TFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TFormWidget(QWidget *parent = nullptr);
    ~TFormWidget();

private slots:
    void on_pushButton_clicked();

signals:
    void title_get(const QString& title);

private:
    Ui::TFormWidget *ui;
};

#endif // TFORMWIDGET_H
