#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>
#include <QTabWidget>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QFileSystemModel>
#include "utilities/qlog.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_bt_calibrate_clicked();

    void on_bt_detect_clicked();

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_actionNew_triggered();

    void on_treeView_customContextMenuRequested(const QPoint &pos);

    void on_actionOpen_triggered();

private:

    bool doConnect();

    bool getFreqArray();

    bool doCalibration();

    bool doDetection();

    void dataRender();

    void table_init();

    bool isQualified();

    void tableClear();

    void dataSync(const QString& filepath, const bool& isAppended);

    void dataOverwrite(const QString& filepath);

    void dataAppend(const QString& filepath);

    bool fetchConfig(const QString& configPath);

    bool fetchDataNRender(const QString& dataPath);

    void checkConfig(const QString& configPath);

private:
    Ui::MainWindow *ui;
    QLog *  log;

private:
    // 常量
    enum { colQC = 0, colDatetime };      // 列字段索引
    const int FIXED_COLUME_NUMS = 2;       // 固定表头列数量(包含Q/C, Timestamp)
    const QString QUALIFIED = "Qualified";
    const QString UNQUALIFIED = "Unqualified";
    QString PROJECTS_PATH;
    const QString DATA_FILE_NAME = "_data.csv";
    const QString CONFIG_FILE_NAME = ".config";

    // 项目变量
    QString project_name;
    // 当前项目目录路径
    QString cur_projects_path;
    QString cur_data_path;
    QString cur_config_path;
    int freq_nums = 10;

    // 模型变量
    QStringList headers;
    // 配置模型
    QJsonObject target_obj;
    // 校准模型
    QJsonObject cali_obj;
    // 测量模型
    QJsonObject detect_obj;

    // file model
    QFileSystemModel* m_model;
};
#endif // MAINWINDOW_H
