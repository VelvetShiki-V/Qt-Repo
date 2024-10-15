#include "tdialogconfig.h"
#include "ui_tdialogconfig.h"

// 辅助函数提取键中的数值和单位
void extractVU(const QString &key, double &value, QString &unit) {
    int pos = key.toLower().indexOf("k");
    if (pos != -1) {
        unit = "K";
        value = key.left(pos).toDouble();
    } else {
        pos = key.toLower().indexOf("m");
        if (pos != -1) {
            unit = "M";
            value = key.left(pos).toDouble();
        }
    }
}

// 自定义频点比较函数
bool compare(const QString &key1, const QString &key2) {
    // 提取单位和数值
    QString unit1, unit2;
    double value1 = 0.0, value2 = 0.0;
    extractVU(key1, value1, unit1);
    extractVU(key2, value2, unit2);
    // 比较单位
    if (unit1 != unit2) {
        // K优先级高于M
        return unit1 == "K" || unit2 == "M";
    }
    // 单位相同则比较数值
    return value1 < value2;
}

// 初始化配置
TDialogConfig::TDialogConfig(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TDialogConfig)
{
    ui->setupUi(this);
    tableInit();
}

// 查看配置（可修改参数但无法改变结构）
TDialogConfig::TDialogConfig(const QString& config_path, const bool& init, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TDialogConfig)
{
    ui->setupUi(this);
    tableInit();
    this->config_path = config_path;
    qDebug() << "get existed config path: " << this->config_path;
    // 解析配置文件并展示
    configRead(this->config_path);
    QStringList freqs = json_config.keys();
    std::sort(freqs.begin(), freqs.end(), compare);
    for(int i = 0; i < json_config.count(); ++i) {
        qDebug() << "key: " << freqs[i] << " val:" << QString::number(json_config[freqs[i]].toDouble());
        dataRender(freqs[i], QString::number(json_config[freqs[i]].toDouble()));
    }
    // 禁用schema结构改变
    if(!init) {
        ui->bt_removeRow->setVisible(false);
        ui->bt_import->setVisible(false);
        ui->bt_appendRow->setVisible(false);
        ui->groupBox->setTitle("alter config (schema is not allowed to alter)!");
        // 遍历所有的行，设置第一列（列索引为 0）的单元格为不可编辑
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *item = ui->tableWidget->item(row, 0); // 获取第一列的单元格
            if (item) {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 去掉可编辑标志
                item->setForeground(QBrush(Qt::red));
            }
        }
    }
}

TDialogConfig::~TDialogConfig()
{
    delete ui;
}

// 导入配置
void TDialogConfig::on_bt_import_clicked()
{
    // 打开配置并解析为json
    json_config = {};
    QString path = QFileDialog::getOpenFileName(this, "open config file", PROJECT_PATH, "配置文件(*.config)");
    configRead(path);
    // 渲染
    QStringList freqs = json_config.keys();
    std::sort(freqs.begin(), freqs.end(), compare);
    for(int i = 0; i < json_config.count(); ++i) {
        qDebug() << "key: " << freqs[i] << " val:" << QString::number(json_config[freqs[i]].toDouble());
        dataRender(freqs[i], QString::number(json_config[freqs[i]].toDouble()));
    }
}

// 应用配置
void TDialogConfig::on_bt_export_clicked()
{
    // 如果工程未选择，则在指定目录新建配置文件
    if(config_path.isEmpty()) {
        QMessageBox::warning(this, "warning", "project not located, please select a config file to apply");
        QString dir_path = QFileDialog::getExistingDirectory(this, "config Save path choose", PROJECT_PATH);
        if(!dir_path.isEmpty()) {
            QString fileName = ".config";
            dir_path += "/" + fileName; // 确保文件名包含在路径中
        }
        qDebug() << "save config file output path: " << dir_path;
        // 创建配置文件
        QFile configFile(dir_path);
        if(configFile.open(QIODevice::WriteOnly)) {
            configFile.write("");
            configFile.close();
        }
        config_path = dir_path;
    }
    // 如果工程已存在(或对新的配置文件)，则对当前配置文件进行覆盖
    QFile conf(config_path);
    if (!conf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "critical", "conf open failed!");
        return;
    }
    QTextStream out(&conf);
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item) {
                out << item->text();
                if (col < ui->tableWidget->columnCount() - 1) {
                    out << ",";
                }
            }
        }
        out << "\n"; // 换行
    }
    // qDebug() << "config rows: " << ui->tableWidget->rowCount();
    qDebug() << "config applied!";
    conf.close();

    // 将表格中的数据存入 QJsonObject
    json_config = {};
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString freqValue, targetValue;
        if(ui->tableWidget->item(row, 0))
            freqValue = ui->tableWidget->item(row, 0)->text();
        if(ui->tableWidget->item(row, 1))
            targetValue = ui->tableWidget->item(row, 1)->text();
        if (!freqValue.isEmpty() && !targetValue.isEmpty()) {
            json_config[freqValue] = targetValue;
        }
        // qDebug() << "total rows: " << ui->tableWidget->rowCount();
        // qDebug() << "scan row: " << row;
        // qDebug() << "get freq: " << freqValue;
        // qDebug() << "get val: " << targetValue;
    }
    qDebug() << "SCAN CONFIG info: " << QJsonDocument(json_config).toJson(QJsonDocument::Compact);
    // 将配置发送给主窗口
    emit config_apply(json_config);
    accept();
}

// 表格渲染
void TDialogConfig::dataRender(const QString& key, const QString& val) {
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    // row start from 1
    int row = ui->tableWidget->rowCount();
    qDebug() << "cur row: " << row;
    QTableWidgetItem* freq = new QTableWidgetItem(key);
    freq->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    freq->setForeground(QBrush(Qt::red));
    // item index start from 0
    ui->tableWidget->setItem(row - 1, 0, freq);
    QTableWidgetItem* target = new QTableWidgetItem(val);
    target->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    ui->tableWidget->setItem(row - 1, 1, target);
}

// 频点表头设置
void TDialogConfig::tableInit() {
    PROJECT_PATH = QCoreApplication::applicationDirPath() + "/../../../../main/projects";
    qDebug() << "tdialogConfig_PROJECT_PATH: " << PROJECT_PATH;
    // 初始化表格
    ui->tableWidget->setColumnCount(2);
    // 设置表头
    QStringList headers;
    headers.append("frequency(/Hz)");
    headers.append("target(/db)");
    for(int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        QTableWidgetItem* header = new QTableWidgetItem();
        QFont font;
        font.setBold(true);
        font.setPointSize(11);
        header->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        header->setFont(font);
        header->setForeground(QBrush(Qt::red));
        header->setText(headers[i]);
        ui->tableWidget->setHorizontalHeaderItem(i, header);
    }
}

// 读取配置文件并转为json object
void TDialogConfig::configRead(const QString &configPath) {
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open file.");
        return;
    }
    QTextStream in(&file);
    // 读取配置
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList keyValue = line.split(",");  // 逗号分割
        if (keyValue.size() == 2) {
            QString key = keyValue[0];          // 读取键
            double value = keyValue[1].toDouble();  // 将值转换为 double
            // 插入到 QJsonObject 中
            json_config.insert(key, value);
        }
    }
    file.close();
}

// 新增一行
void TDialogConfig::on_bt_appendRow_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}

// 删除一行
void TDialogConfig::on_bt_removeRow_clicked()
{
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    ui->bt_removeRow->setEnabled(false);
}

// 选中激活删除行
void TDialogConfig::on_tableWidget_cellClicked(int row, int column)
{
    ui->bt_removeRow->setEnabled(true);
}

