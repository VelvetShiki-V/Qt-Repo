#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tdialogconfig.h"
#include "tdialognewproject.h"
#include "utilities/qlog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), log(new QLog)
{
    ui->setupUi(this);
    log->setupLog(this);
    // 初始化项目管理树
    PROJECTS_PATH = QCoreApplication::applicationDirPath() + "/../../../../main/projects";
    qDebug() << "PROJECTS_PATH" << PROJECTS_PATH;
    m_model = new QFileSystemModel(this);
    m_model->setRootPath(PROJECTS_PATH);
    ui->treeView->setModel(m_model);
    ui->treeView->setRootIndex(m_model->index(PROJECTS_PATH));
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 仅筛选csv文件
    m_model->setNameFilterDisables(false);
    m_model->setNameFilters(QStringList() << "*.csv");

    // 表格的右键菜单初始化和策略（必要）
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    // 允许文件编辑名称
    ui->treeView->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete log;
}

// 辅助函数提取键中的数值和单位
void extractValueAndUnit(const QString &key, double &value, QString &unit) {
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
bool compareKeys(const QString &key1, const QString &key2) {
    // 提取单位和数值
    QString unit1, unit2;
    double value1 = 0.0, value2 = 0.0;
    extractValueAndUnit(key1, value1, unit1);
    extractValueAndUnit(key2, value2, unit2);
    // 比较单位
    if (unit1 != unit2) {
        // K优先级高于M
        return unit1 == "K" || unit2 == "M";
    }
    // 单位相同则比较数值
    return value1 < value2;
}

// 递归删除目录和文件
bool deleteDirectory(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        return false;  // 如果目录不存在，返回 false
    }
    // 遍历目录中的所有内容（文件和子目录）
    foreach (QString entry, dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden)) {
        QString entryPath = path + QDir::separator() + entry;
        QFileInfo entryInfo(entryPath);

        if (entryInfo.isDir()) {
            // 递归删除子目录
            if (!deleteDirectory(entryPath)) {
                return false;  // 如果删除子目录失败，返回 false
            }
        } else {
            // 删除文件
            if (!QFile::remove(entryPath)) {
                return false;  // 如果删除文件失败，返回 false
            }
        }
    }
    // 删除空目录
    return dir.rmdir(path);
}

// 递归拷贝目录及文件
bool copyDirectory(const QString &srcPath, const QString &destPath) {
    QDir srcDir(srcPath);
    if (!srcDir.exists()) {
        return false;  // 如果源目录不存在，返回 false
    }

    // 如果目标目录不存在，则创建
    QDir destDir(destPath);
    if (!destDir.exists()) {
        if (!destDir.mkpath(destPath)) {
            return false;  // 如果创建目标目录失败，返回 false
        }
    }

    // 遍历源目录中的所有文件和子目录
    foreach (QString entry, srcDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        QString srcFilePath = srcPath + QDir::separator() + entry;
        QString destFilePath = destPath + QDir::separator() + entry;
        QFileInfo fileInfo(srcFilePath);

        if (fileInfo.isDir()) {
            // 递归复制子目录
            if (!copyDirectory(srcFilePath, destFilePath)) {
                return false;  // 如果复制子目录失败，返回 false
            }
        } else {
            // 复制文件
            if (!QFile::copy(srcFilePath, destFilePath)) {
                return false;  // 如果复制文件失败，返回 false
            }
        }
    }
    return true;  // 全部复制成功，返回 true
}

// 初始化表格头部UI
void MainWindow::table_init() {
    // 根据配置文件target_obj进行表头设置
    // 设置表头
    headers << "Q/C" << "Timestamp";
    QStringList keys = target_obj.keys();
    // 频点单位排序
    std::sort(keys.begin(), keys.end(), compareKeys);
    for(const auto& key: keys) {
        headers.append(key);
    }
    qDebug() << "table headers: " << headers;
    ui->tableWidget->setColumnCount(headers.count());
    // 将表头应用至表格
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
    // 设置行列
    // ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(headers.count());
}

// 点击校准触发事件
void MainWindow::on_bt_calibrate_clicked() {
    cali_obj = {};
    // 检查配置
    if(target_obj.isEmpty()) {
        QMessageBox::critical(this, "critical", "config not set yet!");
        return;
    }
    // 1. 连接机器
    if(!doConnect()) {
        QMessageBox::critical(this, "critical", "connection error!");
        return;
    }
    // 2. 校准并设置cali_obj
    if(!doCalibration()) {
        QMessageBox::critical(this, "critical", "calibration error!");
        return;
    }
    qDebug() << "get calibration obj: " << QJsonDocument(cali_obj).toJson(QJsonDocument::Compact);
    // 校准完成
    ui->cb_calibrate->setCheckState(Qt::Checked);
    // ui->bt_calibrate->setText("重新校准");
    QMessageBox::information(this, "info", "calibration success!");
}

// 点击检测触发事件
void MainWindow::on_bt_detect_clicked() {
    detect_obj = {};
    // 检查配置
    if(target_obj.isEmpty()) {
        QMessageBox::critical(this, "critical", "config not set yet!");
        return;
    }
    // 1. 连接机器
    if(!doConnect()) {
        QMessageBox::critical(this, "critical", "machine connection error!");
        return;
    }
    // 2. 检查是否校准
    if(cali_obj.isEmpty()) {
        QMessageBox::warning(this, "warning", "you haven't calibrated yet!");
        return;
    }
    // 3. 检测
    if(!doDetection()) {
        QMessageBox::warning(this, "critical", "detection error!");
        return;
    }
    // 4. 处理检测结果
    qDebug() << "get detection result: " << QJsonDocument(detect_obj).toJson(QJsonDocument::Compact);
    // 5. 渲染
    dataRender();
    // 6. 同步数据持久化
    dataSync(cur_data_path, true);
}

// 根据检测值渲染表格
void MainWindow::dataRender() {
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    // 获取当前行数
    int row = ui->tableWidget->rowCount();
    // 填充数据
    // 质检
    QTableWidgetItem* itemQC = new QTableWidgetItem();
    if(isQualified()) {
        itemQC->setText(QUALIFIED);
        itemQC->setBackground(QBrush(QColor(178, 255, 143)));
        itemQC->setCheckState(Qt::Checked);
    }
    else {
        itemQC->setText(UNQUALIFIED);
        itemQC->setCheckState(Qt::Unchecked);
        QFont font;
        font.setBold(true);
        itemQC->setForeground(QBrush(Qt::red));
        itemQC->setFont(font);
        itemQC->setBackground(QBrush(QColor(255, 182, 193)));
    }
    itemQC->setFlags(Qt::ItemIsEnabled);
    itemQC->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    // item index start from 0
    ui->tableWidget->setItem(row - 1, colQC, itemQC);
    // 时间
    QTableWidgetItem* itemTime = new QTableWidgetItem();
    if(detect_obj["Timestamp"].isNull()) {
        itemTime->setText(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    } else itemTime->setText(detect_obj["Timestamp"].toString());
    itemTime->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    ui->tableWidget->setItem(row - 1, colDatetime, itemTime);
    // 频点
    for(int i = FIXED_COLUME_NUMS; i < headers.count(); ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(detect_obj[headers[i]].toDouble()));
        // qDebug() << "key: " << headers[i] << "val: " << QString::number(detect_obj[headers[i]].toDouble());
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        // 比大小，不合规标红
        if(detect_obj[headers[i]].toDouble() < target_obj[headers[i]].toDouble()) {
            item->setForeground(QBrush(Qt::red));
            item->setBackground(QBrush(QColor(255, 182, 193)));
        }
        ui->tableWidget->setItem(row - 1, i, item);
    }
    // 默认选中最新数据
    QItemSelectionModel* selectionModel = ui->tableWidget->selectionModel();
    if(selectionModel) {
        selectionModel->clearSelection();
        QModelIndex tl = ui->tableWidget->model()->index(row - 1, 0);
        QModelIndex br = ui->tableWidget->model()->index(row - 1, ui->tableWidget->columnCount() - 1);
        // 选中
        QItemSelection selection(tl, br);
        selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
    // 翻滚至最后
    ui->tableWidget->scrollToBottom();
}

// 检测值与预定值比较
bool MainWindow::isQualified() {
    // 遍历所有的键
    for (const QString &key : target_obj.keys()) {
        // 检查detectObj是否包含当前键
        if (detect_obj.contains(key)) {
            // 获取两个对象中对应键的值
            double targetValue = target_obj[key].toDouble();
            double detectValue = detect_obj[key].toDouble();
            // 如果detectObj中的值小于targetObj中的值，返回false; 如果detectObj不包含targetObj的某个键，也返回false
            if (detectValue < targetValue) return false;
        } else {
            qDebug() << "detect target obj does not match, cannot compare";
            return false;
        }
    }
    // 如果所有的值都满足条件，返回true
    return true;
}

// 数据同步持久化
void MainWindow::dataSync(const QString& filepath, const bool& isAppended) {
    // 如果文件不存在，弹出对话框创建文件
    // 如果文件为空，默认执行覆盖操作
    QFile dataFile(filepath);
    if(dataFile.open(QIODevice::ReadOnly)) {
        if(dataFile.size() == 0) {
            qDebug() << "file is empty and will be covered";
            dataOverwrite(filepath);
            dataFile.close();
            return;
        }
    }
    dataFile.close();
    // 覆盖或追加
    if(!isAppended) dataOverwrite(filepath);
    else dataAppend(filepath);
}

// 数据覆写(带表头)
void MainWindow::dataOverwrite(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "warning", "file open failed!");
        return;
    }
    QTextStream out(&file);
    // 写入表头
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        if (ui->tableWidget->horizontalHeaderItem(i)) {
            out << ui->tableWidget->horizontalHeaderItem(i)->text();
        } else {
            out << "Column" << i; // 如果没有表头，使用默认名称
        }
        if (i < ui->tableWidget->columnCount() - 1) {
            out << ",";
        }
    }
    out << "\n"; // 换行
    // 遍历每一行和每一列
    for (int row = 0; row < ui->tableWidget->rowCount() - 1; ++row) {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item) {
                out << item->text();
            } else {
                out << ""; // 如果单元格为空，写入空字符串
            }
            if (col < ui->tableWidget->columnCount() - 1) {
                out << ",";
            }
        }
        out << "\n"; // 换行
    }
    qDebug() << "data covered";
    file.close();
}

// 数据追加
void MainWindow::dataAppend(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::warning(this, "warning", "file open failed!");
        return;
    }
    QTextStream out(&file);
    // 将最后一行数据内容追加到csv最后(-2因为渲染时新插入了一行，所以-2为当前最后一行数据)
    int row = ui->tableWidget->rowCount() - 2;
    // qDebug() << "current row: " << row;
    for(int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = ui->tableWidget->item(row, col);
        if (item) {
            // qDebug() << "item exists: " << item->text();
            out << item->text();
        } else {
            qDebug() << "item does not exist";
            out << ""; // 如果单元格为空，写入空字符串
        }
        if (col < ui->tableWidget->columnCount() - 1) {
            out << ",";
        }
    }
    out << "\n"; // 换行
    qDebug() << "data appended";
    file.close();
}

// 表格重置
void MainWindow::tableClear()
{
    // UI重置
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    headers.clear();
}

// 双击触发表格预览事件
void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if (!m_model->filePath(index).endsWith(".csv")) return;
    // 重置项目
    ui->cb_config->setCheckState(Qt::Unchecked);
    ui->cb_calibrate->setCheckState(Qt::Unchecked);
    tableClear();
    target_obj = {};
    cali_obj = {};
    detect_obj = {};
    cur_projects_path = "";
    cur_config_path = "";
    cur_data_path = "";
    // 获取项目路径和配置文件路径
    cur_data_path = m_model->filePath(index);
    qDebug() << "tree index csv file path: " << cur_data_path;
    cur_projects_path = cur_data_path.section("/" + DATA_FILE_NAME, 0, 0);
    cur_config_path = cur_projects_path + "/" + CONFIG_FILE_NAME;
    qDebug() << "preview project path: " << cur_projects_path;
    qDebug() << "preview data path: " << cur_data_path;
    qDebug() << "preview config path: " << cur_config_path;

    // 切换配置
    if(!fetchConfig(cur_config_path)) {
        QMessageBox::critical(this, "critical", "config shift fail! Please check if config file is valid inside the project");
        return;
    }

    // 读取csv文件并渲染
    if(!fetchDataNRender(cur_data_path)) {
        QMessageBox::critical(this, "critical", "config shift fail! Please check if config file is valid inside the project");
    }

    ui->cb_config->setCheckState(Qt::Checked);
}

// 读取配置文件并设置target
bool MainWindow::fetchConfig(const QString &configPath)
{
    target_obj = {};
    QFile conf(configPath);
    if (!conf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open config file. Please create new config!");
        return false;
    }
    QTextStream in(&conf);
    // 读取配置
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList keyValue = line.split(",");  // 逗号分割
        if (keyValue.size() == 2) {
            QString key = keyValue[0];          // 读取键
            double value = keyValue[1].toDouble();  // 将值转换为 double
            // 插入到 QJsonObject 中
            target_obj.insert(key, value);
        }
    }
    if(target_obj.isEmpty()) return false;

    // 配置展示或修改
    // TDialogConfig* tdConfig = new TDialogConfig(target_obj, cur_projects_path + "/" + CONFIG_FILE_NAME);
    // if(tdConfig->exec() == QDialog::Accepted) {
    //     qDebug() << "config set success: " << target_obj;
    //     qDebug() << "config path get : " << cur_projects_path;
    // } else {
    //     conf.close();
    //     return;
    // }

    conf.close();
    qDebug() << "config shift success: " << target_obj;
    QMessageBox::information(this, "success", "config shift success");
    return true;
}

// 读取数据文件并渲染
bool MainWindow::fetchDataNRender(const QString &dataPath)
{
    QFile data(dataPath);
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Fatal", "Can't open csv file");
        qDebug() << "can't open file" << dataPath;
        return false;
    }
    // 内容解析
    QTextStream ins(&data);
    // 键解析
    QString firstLine = ins.readLine();  // 第一行是键
    QStringList keys = firstLine.split(",");  // 将键存储到列表中
    ins.setCodec("UTF-8"); // 根据文件编码设置
    // 初始化表格准备读取解析并渲染内容
    table_init();
    // 值解析
    while(!ins.atEnd()) {
        QJsonObject loaded_obj;
        QString line = ins.readLine();
        QStringList values = line.split(",");
        // 验证格式合法性
        if (values.size() != keys.size()) {
            QMessageBox::warning(this, "Error", "CSV format error: keys and values size do not match.");
            return false;
        }
        // 逐行解析
        for (int i = 0; i < keys.size(); ++i)
        {
            QString key = keys[i];
            QString value = values[i];

            if (key == "Q/C" || key == "Timestamp") {
                // Q/C 列是 QString 类型
                loaded_obj.insert(key, value);
            } else {
                // 其余列是 double 类型
                double numValue = value.toDouble();
                loaded_obj.insert(key, numValue);
            }
        }
        // 调试观察
        // QJsonDocument doc(loaded_obj);
        // QString jsonString = doc.toJson(QJsonDocument::Compact);
        // qDebug() << "csv parse line ret: " <<jsonString;

        //逐行渲染
        detect_obj = loaded_obj;
        dataRender();
    }
    data.close();
    return true;
}

// 预览已存在配置
void MainWindow::checkConfig(const QString& configPath)
{
    qDebug() << "config path get: " << configPath;
    TDialogConfig* config_dialog = new TDialogConfig(configPath, false);
    config_dialog->setWindowFlag(Qt::WindowStaysOnTopHint);
    // 注册回调
    connect(config_dialog, &TDialogConfig::config_apply, this,
            [this](const QJsonObject& config_target){
                // 配置加载
                target_obj = config_target;
                // 状态变更
                ui->cb_config->setChecked(true);
            });
    if(config_dialog->exec() == QDialog::Accepted) {
        qDebug() << "config set success: " << target_obj;
    }
}

// 机器连接
bool MainWindow::doConnect() {
    // TODO: 连接机器
    return true;
}

// 机器校准
bool MainWindow::doCalibration() {
    // TODO: 通过机器连接获取校准值
    // cali_obj = {
    //     {"10K", -2.047},
    //     {"50K", -2.047},
    //     {"100K", -13.1352},
    //     {"150K", -12.523},
    //     {"500K", -12.523},
    //     {"1M", -17.1454},
    //     {"5M", -17.1454},
    //     {"30M", -8.105},
    //     {"50M", -8.105},
    //     {"100M", -2.305}
    // };
    cali_obj = target_obj;
    return true;
}

// 数据检测
bool MainWindow::doDetection() {
    // TODO: 通过机器获取数据
    // 测量模型
    detect_obj = target_obj;
    // 不合格
    // detect_obj = {
    //     {"10K", 3.047},
    //     {"50K", 5.047},
    //     {"100K", 7.1352},
    //     {"150K", 4.523},
    //     {"500K", 13.523},
    //     {"1M", 1.1454},
    //     {"5M", 21.1454},
    //     {"30M", 8.105},
    //     {"50M", 8.105},
    //     {"100M", 30.305}
    // };
    // 合格
    // detect_obj = {
    //     {"10K", 3.047},
    //     {"50K", 5.047},
    //     {"100K", 7.1352},
    //     {"150K", 10.523},
    //     {"500K", 99.523},
    //     {"1M", 99.1454},
    //     {"5M", 99.1454},
    //     {"30M", 99.105},
    //     {"50M", 99.105},
    //     {"100M", 99.305}
    // };
    return true;
}

// 新建工程事件触发
void MainWindow::on_actionNew_triggered()
{
    // 在projects目录下新建目录并创建默认.config配置文件和数据文件
    TDialogNewProject* dialogName = new TDialogNewProject();
    dialogName->setWindowFlag(Qt::WindowStaysOnTopHint);
    // 注册回调
    connect(dialogName, &TDialogNewProject::project_name_apply, this, [this](const QString& name){ project_name = name; });
    if(dialogName->exec() == QDialog::Accepted) {
        cur_projects_path = PROJECTS_PATH + "/" + project_name;
        qDebug() << "new project dir path get: " << cur_projects_path;
        // 检查目录是否存在，否则创建
        QDir dir;
        if(!dir.exists(cur_projects_path)) {
            if(dir.mkdir(cur_projects_path)) {
                // 目录创建成功，创建默认文件
                // .config隐藏文件
                cur_config_path = cur_projects_path + "/" + CONFIG_FILE_NAME;
                QFile configFile(cur_config_path);
                if(configFile.open(QIODevice::WriteOnly)) {
                    configFile.write("");
                    configFile.close();
                }
                // project.csv文件
                cur_data_path = cur_projects_path + "/" + DATA_FILE_NAME;
                QFile datFile(cur_data_path);
                if(datFile.open(QIODevice::WriteOnly)) {
                    datFile.write("");
                    datFile.close();
                }
                QMessageBox::information(this, "success", "project create success!");
            } else {
                QMessageBox::critical(this, "critical", "project create failed!");
            }
        } else {
            QMessageBox::warning(this, "warning", "project has already existed!");
        }
        // 目录及文件创建后进行初始化配置
        TDialogConfig* dialogConfig = new TDialogConfig(cur_config_path, true);
        dialogConfig->setWindowFlag(Qt::WindowStaysOnTopHint);
        connect(dialogConfig, &TDialogConfig::config_apply, this, [this](const QJsonObject& config) { target_obj = config; });
        if(dialogConfig->exec() == QDialog::Accepted) {
            // 配置完成
            qDebug() << project_name << " config init success: " << target_obj;
        }
    }
}

// 右键事件
void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (index.isValid() && m_model->isDir(index)) {
        // 已有工程操作（包含重命名, 删除工程）
        QMenu* list = new QMenu(this);
        // rename
        QAction* renameAction = new QAction("rename", this);
        connect(renameAction, &QAction::triggered, this, [=]() { ui->treeView->edit(index); });
        list->addAction(renameAction);
        // remove
        QAction* removeAction = new QAction("remove", this);
        connect(removeAction, &QAction::triggered, this, [=]() {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "remove confirm", "remove this project?",
                                          QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::Yes) {
                // 删除逻辑
                // qDebug() << "dir: " << m_model->filePath(index) << " will be removed!";
                deleteDirectory(m_model->filePath(index));
            }
        });
        list->addAction(removeAction);
        // export
        QAction* importAction = new QAction("export project", this);
        connect(importAction, &QAction::triggered, this, [=]() {
            QString export_path = QFileDialog::getExistingDirectory(this, "Project export", QDir::rootPath());
            copyDirectory(m_model->filePath(index), export_path);
        });
        list->addAction(importAction);
        list->exec(ui->treeView->mapToGlobal(pos));
    } else if(m_model->filePath(index).endsWith(".csv")) {
        QMenu* list = new QMenu(this);
        // 已有数据配置（配置文件改变, 一键清空文件）
        QAction* configCheckAction = new QAction("checkout config", this);
        QString config_path = m_model->filePath(index).replace("/_data.csv", "/.config");
        connect(configCheckAction, &QAction::triggered, this, [=]() { checkConfig(config_path); });
        list->addAction(configCheckAction);
        list->exec(ui->treeView->mapToGlobal(pos));
    } else {
        // 新建工程（或导入工程）
        QMenu* list = new QMenu(this);
        // create
        QAction* createAction = new QAction("create a new project", this);
        connect(createAction, &QAction::triggered, this, &MainWindow::on_actionNew_triggered);
        list->addAction(createAction);
        // import
        QAction* importAction = new QAction("import a new project", this);
        connect(importAction, &QAction::triggered, this, &MainWindow::on_actionOpen_triggered);
        list->addAction(importAction);
        list->exec(ui->treeView->mapToGlobal(pos));
    }
}

// 导入工程
void MainWindow::on_actionOpen_triggered()
{
    // 1. 打开弹窗选择路径
    QString import_path = QFileDialog::getExistingDirectory(this, "Project import", QDir::rootPath());
    copyDirectory(import_path, PROJECTS_PATH);
}
