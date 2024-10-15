#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化tableview模型
    m_model = new QStandardItemModel(4, 4, this);
    m_selection = new QItemSelectionModel(m_model);
    ui->tableView->setModel(m_model);
    ui->tableView->setSelectionModel(m_selection);
    // 初始化表格内容（默认表头，4行4列）
    QStringList header;
    header << "姓名" << "年龄" << "班级" << "ID";
    m_model->setHorizontalHeaderLabels(header);
    dialog_header = nullptr;
    // 初始化状态栏标签组件
    cell_pos = new QLabel("当前位置", this);
    cell_pos->setMinimumWidth(200);
    cell_content = new QLabel("当前内容", this);
    cell_pos->setMinimumWidth(200);
    ui->statusbar->addWidget(cell_pos);
    ui->statusbar->addWidget(cell_content);
    // 槽函数：当选中表格不同区域时，状态栏标签发生变化
    connect(m_selection, &QItemSelectionModel::currentChanged, this, &MainWindow::current_item_changed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 设置表格行列数（跳转到自定义对话框，通过模型进行设置）
void MainWindow::on_action_set_row_column_triggered()
{
    TDialogTableView* dialog_table_view = new TDialogTableView();
    dialog_table_view->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);     // 对话框大小固定（不能缩放）
    // 将当前模型的表格行列数同步给对话框
    dialog_table_view->set_column_rows(m_model->columnCount(), m_model->rowCount());
    int ret = dialog_table_view->exec();        // exec以执行并显示该对话框
    if(ret == QDialog::Accepted) {
        // 将对话框设置的行列数同步给当前表格
        m_model->setColumnCount(dialog_table_view->get_column_changed());
        m_model->setRowCount(dialog_table_view->get_row_changed());

    }
    delete dialog_table_view;
}


void MainWindow::on_action_set_header_triggered()
{
    if(dialog_header == nullptr)
        dialog_header = new TDialogHeaderList(this);
    QStringList header;
    // 遍历列取出当前表头
    for(int i = 0; i < m_model->columnCount(); ++i) {
        header += m_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
    }
    dialog_header->get_current_header_list(header);
    int ret = dialog_header->exec();
    if(ret == QDialog::Accepted) {
        m_model->setHorizontalHeaderLabels(dialog_header->set_changed_list());
    }
}


void MainWindow::on_action_locate_triggered()
{
    TDialogLocate* dialog_locate = new TDialogLocate(this);
    dialog_locate->setAttribute(Qt::WA_DeleteOnClose);      // 关闭时自动删除该对象
    dialog_locate->setWindowFlag(Qt::WindowStaysOnTopHint);     // 对话框顶层显示，可与show并用方便观察
    dialog_locate->set_spin_range(m_model->rowCount(), m_model->columnCount());
    QModelIndex index = m_selection->currentIndex();
    if(index.isValid())
        // 将当前选中的单元格坐标传递到对话框的spinbox中并初始化值
        dialog_locate->get_row_column(index.row(), index.column());
    // 三个触发的槽函数分别对应（1. 文本修改  2. 定位功能按钮启用  3. 点击单元格实时更新spinbox数值）
    connect(dialog_locate, &TDialogLocate::text_change, this, &MainWindow::set_text);
    connect(dialog_locate, &TDialogLocate::locate_button_enable, ui->action_locate, &QAction::setEnabled);
    connect(this, &MainWindow::tell_current_row_column, dialog_locate, &TDialogLocate::get_row_column);
    dialog_locate->show();      // 不能使用exec，否则对话框优先级最高显示
    dialog_locate->setModal(false);     // 非模态运行对话框（？

}

// 槽函数方法，接收到对话框传来的信号和内容，进入该方法并给tableView赋值
void MainWindow::set_text(int row, int column, QString content)
{
    QModelIndex index = m_model->index(row, column);
    m_selection->clearSelection();
    m_selection->setCurrentIndex(index, QItemSelectionModel::Select);
    m_model->setData(index, content, Qt::DisplayRole);
}

// 槽函数信号，点击不同单元格时将当前坐标信号发送给对话框spinbox
void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    emit tell_current_row_column(index.row(), index.column());
}

// 槽函数方法，点击不同单元格时将获取到的当前坐标赋值给label在状态栏显示
void MainWindow::current_item_changed(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(current.isValid()) {
        cell_pos->setText(QString("所在行:%1\t所在列:%2").arg(current.row()).arg(current.column()));
        cell_content->setText(m_model->itemFromIndex(current)->text());
    }
}

