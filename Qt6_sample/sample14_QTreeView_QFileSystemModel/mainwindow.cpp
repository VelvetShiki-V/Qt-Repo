#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 为不同的view组件设置model以进行模块控制
    m_model = new QFileSystemModel(this);
    ui->treeView->setModel(m_model);
    ui->listView->setModel(m_model);
    ui->tableView->setModel(m_model);

    // 界面刚打开时设置的根路径显示行为（将treeView视图与listView, tableView联动）
    m_model->setRootPath("");
    connect(ui->treeView, &QTreeView::clicked, ui->listView, &QListView::setRootIndex);
    connect(ui->treeView, &QTreeView::clicked, ui->tableView, &QTableView::setRootIndex);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_root_dir_triggered()
{
    // 获取目录路径（QDir设置从当前exe路径开始浏览）
    QString dir_path = QFileDialog::getExistingDirectory(this, "选择目录", QDir::currentPath());
    if(dir_path.isEmpty()) return;
    // model设置根目录径
    m_model->setRootPath(dir_path);
    // 根据model设置的路径，三个view组件同时显示
    ui->treeView->setRootIndex(m_model->index(dir_path));
    ui->listView->setRootIndex(m_model->index(dir_path));
    ui->tableView->setRootIndex(m_model->index(dir_path));
}


// 只显示目录
void MainWindow::on_radioButton_only_dir_clicked()
{
    ui->groupBox_filter->setEnabled(false);
    m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);       // 所有目录和非上一级目录显示
}


// 显示所有文件（或过滤文件）
void MainWindow::on_radioButton_all_clicked()
{
    ui->groupBox_filter->setEnabled(true);
    // 模型同步可筛选状态
    m_model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
}

// 是否筛选（调整按钮启用状态）
void MainWindow::on_checkBox_filter_clicked(bool checked)
{
    ui->comboBox->setEnabled(checked);
    ui->pushButton_apply->setEnabled(checked);
    m_model->setNameFilterDisables(!checked);
}


void MainWindow::on_pushButton_apply_clicked()
{
    // 根据下拉框的文件后缀进行筛选
    QStringList file_types = ui->comboBox->currentText().split(";", Qt::SkipEmptyParts);
    m_model->setNameFilters(file_types);
}

// 状态栏显示
void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    ui->label_filename->setText(m_model->fileName(index));
    ui->label_filepath->setText(m_model->filePath(index));
    ui->label_filetype->setText(m_model->type(index));
    ui->checkBox_is_dir->setChecked(m_model->isDir(index));
    int size = m_model->size(index) / 1024;
    if(size < 1024) {
        ui->label_filesize->setText(QString("%1 Kb").arg(size));
    }
    else
        ui->label_filesize->setText(QString("%1 Mb").arg(size / 1024));
}

