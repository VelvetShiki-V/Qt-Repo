#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 属性初始化
    cell_pos = new QLabel("单元格位置", this);
    cell_pos->setMinimumWidth(200);
    cell_content = new QLabel("单元格内容", this);
    cell_content->setMinimumWidth(200);
    file_name = new QLabel("打开的文件名", this);
    file_name->setMinimumWidth(200);
    m_model = new QStandardItemModel(2, column_num, this);      // 2无意义，初始化用
    m_selection = new QItemSelectionModel(m_model, this);

    // 状态栏增加标签组件
    ui->statusbar->addWidget(cell_pos);
    ui->statusbar->addWidget(cell_content);
    ui->statusbar->addWidget(file_name);

    // tableview由model控制
    ui->tableView->setModel(m_model);
    ui->tableView->setSelectionModel(m_selection);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);      // 可拖拽选择
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

    // 控制单击时statusbar显示的内容格式
    connect(m_selection, &QItemSelectionModel::currentChanged, this, &MainWindow::do_currentItemChanged);

    // 动作组
//    QActionGroup* align = new QActionGroup(this);
//    align->addAction(ui->action_left);
//    align->addAction(ui->action_center);
//    align->addAction(ui->action_right);

    // 代理初始化
    spin_dlg = new TSpinboxDelegate(this);
    ui->tableView->setItemDelegateForColumn(0, spin_dlg);

    dspin_dlg = new TDoubleSpinboxDelegate(this);
    ui->tableView->setItemDelegateForColumn(1, dspin_dlg);
    ui->tableView->setItemDelegateForColumn(2, dspin_dlg);
    ui->tableView->setItemDelegateForColumn(3, dspin_dlg);

    combo_dlg = new TComboBoxDelegate(this);
    QStringList list;
    list << "优" << "良" << "中" << "差";
    combo_dlg->set_list_content(list, false);
    ui->tableView->setItemDelegateForColumn(4, combo_dlg);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// 将从txt中读取到的每行信息初始化到table view上
void MainWindow::modelInit(const QStringList &file_contents)
{
    // 提取表头（第0行数据）
    QString header = file_contents[0];
    QStringList head_parts = header.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
    m_model->setHorizontalHeaderLabels(head_parts);

    // 按行提取表行信息
    for(int i = 1; i < file_contents.size(); ++i) {
        QString line = file_contents[i];
        QStringList line_parts = line.split(QRegularExpression(R"(\s+)"), Qt::SkipEmptyParts);
        QStandardItem* item;
        int j;
        // 每一行的每一列单独提取到对应item中
        for(j = 0; j < line_parts.size() - 1; ++j) {
            item = new QStandardItem(line_parts[j]);
            m_model->setItem(i - 1, j, item);
        }
        // 最后一列为复选框，单独处理
        item = new QStandardItem(head_parts[j]);        // 显示内容即为标头内容，而非0或1
        item->setBackground(QBrush(Qt::yellow));
        item->setCheckable(true);
        if(line_parts[j] == "0") item->setCheckState(Qt::Unchecked);
        else item->setCheckState(Qt::Checked);
        m_model->setItem(i - 1, j, item);
    }
}

// 鼠标单击中不同单元格时，状态栏显示的信息
void MainWindow::do_currentItemChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(current.isValid()) {
        cell_pos->setText(QString("当前行:%1, 当前列:%2").arg(current.row()).arg(current.column()));
        QStandardItem* item = m_model->itemFromIndex(current);
        cell_content->setText("读取内容: " + item->text());
    }
}


void MainWindow::on_action_open_file_triggered()
{
    // 打开文件
    QString app_path = QCoreApplication::applicationDirPath();      // 从当前程序运行目录打开文件选择对话框
    QString file_path = QFileDialog::getOpenFileName(this, "选择文件", app_path,
                                                     "数据文件(*.txt);;全部文件(*.*)");
    QFile file(file_path);
    // 文件打开检测
    if(app_path.isEmpty() || file_path.isEmpty() ||
      !file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    file_name->setText("当前文件: " + file_path);
    // 按行读取到plainTextEditor
    QStringList row_content;
    QTextStream stream(&file);
    ui->plainTextEdit->clear();
    while(!stream.atEnd()) {        // 文件流，按行读取
        QString line = stream.readLine();
        ui->plainTextEdit->appendPlainText(line);
        row_content += line;
    }
    file.close();
    // 将表格信息初始化到table view中
    modelInit(row_content);
}

// 将表格数据反向显示到文本框
void MainWindow::on_action_data_view_triggered()
{
    ui->plainTextEdit->clear();
    QString display;
    QStandardItem* item;
    // 先将表头提取显示
    for(int i = 0; i < m_model->columnCount(); ++i) {
        item = m_model->horizontalHeaderItem(i);
        display += item->text() + "\t";
    }
    ui->plainTextEdit->appendPlainText(display);
    // 按行顺序显示内容
    for(int i = 0; i < m_model->rowCount(); ++i) {
        int j;
        display.clear();
        for(j = 0; j < m_model->columnCount() - 1; ++j) {
            item = m_model->item(i, j);
            display += item->text() + "\t";
        }
        item = m_model->item(i, m_model->columnCount() - 1);
        if(item->checkState() == Qt::Checked) display += "是";
        else display += "否";
        ui->plainTextEdit->appendPlainText(display);
    }
}

// 添加行
void MainWindow::on_action_add_row_triggered()
{
    QList<QStandardItem*> list;     // insert需要QList参数
    QStandardItem* item;
    // 前n列初始化默认为new
    for(int i = 0; i < m_model->columnCount() - 1; ++i) {
        item = new QStandardItem("New");
        list << item;
    }
    // 最后一列单独处理
    QString str = m_model->headerData(m_model->columnCount() - 1, Qt::Horizontal).toString();
    item = new QStandardItem(str);
    item->setBackground(QBrush(Qt::yellow));
    item->setCheckable(true);
    m_model->insertRow(m_model->rowCount(), list);
    // 插入后的默认选择单元
    m_selection->clearSelection();
    m_selection->setCurrentIndex(m_model->index(m_model->rowCount() - 1, 0), QItemSelectionModel::Select);
}


void MainWindow::on_action_insert_row_triggered()
{
    QList<QStandardItem*> list;     // insert需要QList参数
    QStandardItem* item;
    for(int i = 0; i < m_model->columnCount() - 1; ++i) {
        item = new QStandardItem("New");
        list << item;
    }
    // 最后一列单独处理
    QString str = m_model->headerData(m_model->columnCount() - 1, Qt::Horizontal).toString();
    item = new QStandardItem(str);
    item->setBackground(QBrush(Qt::yellow));
    item->setCheckable(true);
    QModelIndex current_index = m_selection->currentIndex();
    m_model->insertRow(current_index.row(), list);
    // 插入后的默认选择单元
    m_selection->clearSelection();
    m_selection->setCurrentIndex(m_model->index(current_index.row(), 0), QItemSelectionModel::Select);
}


void MainWindow::on_action_delete_row_triggered()
{
    QModelIndex index = m_selection->currentIndex();
    if(index.row() != m_model->rowCount() - 1) {
        m_model->removeRow(index.row());
        // 非最后一行删除后的单元格选择状态
        m_selection->setCurrentIndex(m_model->index(index.row(), 0), QItemSelectionModel::Select);
    }
    else {
        m_model->removeRow(index.row());
        // 最后一行删除后默认选择最后一行第一列单元格
        m_selection->setCurrentIndex(m_model->index(m_model->rowCount() - 1, 0), QItemSelectionModel::Select);
    }
}


void MainWindow::on_action_left_triggered()
{
    if(!m_selection->hasSelection()) return;
    else {
        QModelIndexList indexs = m_selection->selectedIndexes();
        for(auto& e: indexs) {
            m_model->itemFromIndex(e)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }
}


void MainWindow::on_action_center_triggered()
{
    if(!m_selection->hasSelection()) return;
    else {
        QModelIndexList indexs = m_selection->selectedIndexes();
        for(auto& e: indexs) {
            m_model->itemFromIndex(e)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}


void MainWindow::on_action_right_triggered()
{
    if(!m_selection->hasSelection()) return;
    else {
        QModelIndexList indexs = m_selection->selectedIndexes();
        for(auto& e: indexs) {
            m_model->itemFromIndex(e)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
}

void MainWindow::on_action_bold_triggered(bool checked)
{
    if(!m_selection->hasSelection()) return;
    else {
        QModelIndexList indexs = m_selection->selectedIndexes();
        for(auto& e: indexs) {
            QFont font = m_model->itemFromIndex(e)->font();
            font.setBold(checked);
            m_model->itemFromIndex(e)->setFont(font);
        }
    }
}

