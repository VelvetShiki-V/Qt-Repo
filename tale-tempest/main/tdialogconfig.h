#ifndef TDIALOGCONFIG_H
#define TDIALOGCONFIG_H

#include <QDialog>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>

namespace Ui {
class TDialogConfig;
}

class TDialogConfig : public QDialog
{
    Q_OBJECT

public:
    explicit TDialogConfig(QWidget *parent = nullptr);
    TDialogConfig(const QString& config_path, const bool& init, QWidget *parent = nullptr);
    ~TDialogConfig();

private slots:
    void on_bt_import_clicked();

    void on_bt_export_clicked();

    void on_bt_appendRow_clicked();

    void on_bt_removeRow_clicked();

    void on_tableWidget_cellClicked(int row, int column);


private:
    Ui::TDialogConfig *ui;
    void dataRender(const QString& key, const QString& val);
    void tableInit();
    void configRead(const QString& configPath);

private:
    // 加载的配置内容
    QJsonObject json_config;
    QString config_path;
    // 配置路径
    QString PROJECT_PATH;

signals:
    void config_apply(const QJsonObject& config);
};

#endif // TDIALOGCONFIG_H
