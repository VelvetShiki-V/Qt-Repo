#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 将动作与toolButton关联
    ui->toolButton_init->setDefaultAction(ui->action_initializer);
    ui->toolButton_add->setDefaultAction(ui->action_add);
    ui->toolButton_clear->setDefaultAction(ui->action_clear);
    ui->toolButton_insert->setDefaultAction(ui->action_insert);
    ui->toolButton_delete->setDefaultAction(ui->action_delete);

    ui->toolButton_all->setDefaultAction(ui->action_all);
    ui->toolButton_empty->setDefaultAction(ui->action_none);
    ui->toolButton_inv->setDefaultAction(ui->action_inv);

    // toolbutton设置下拉按钮功能
    QMenu *popup = new QMenu(this);
    popup->addAction(ui->action_all);
    popup->addAction(ui->action_none);
    popup->addAction(ui->action_inv);
    ui->toolButton_menu->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolButton_menu->setMenu(popup);

    // ui添加工具栏下拉按钮
    QToolButton *button_popup = new QToolButton(this);
    button_popup->setPopupMode(QToolButton::MenuButtonPopup);
    button_popup->setText("功能选项");
    button_popup->setIcon(QIcon(":/images/icons/12.png"));
    button_popup->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button_popup->setMenu(popup);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(button_popup);

    // 退出功能
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->action_quit);

}

MainWindow::~MainWindow()
{
    delete ui;
}

// 1. 初始化列表（默认放10个，带图标）
void MainWindow::on_action_initializer_triggered()
{
    QListWidgetItem *item;
    ui->listWidget->clear();
    QIcon icon(":/images/icons/13.png");
    for(int i = 0; i < 10; ++i) {
        item = new QListWidgetItem();
        item->setText("Item" + QString::number(i));
        item->setIcon(icon);
        item->setCheckState(Qt::Checked);
        if(ui->checkBox_editable->isChecked())
            item->setFlags(Qt::ItemIsSelectable | Qt:: ItemIsUserCheckable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
        else
        item->setFlags(Qt::ItemIsSelectable | Qt:: ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui->listWidget->addItem(item);
    }
}

// 动作触发槽函数——动作定义利于不同按钮功能的复用
void MainWindow::on_action_clear_triggered()
{
    ui->listWidget->clear();
}


void MainWindow::on_action_insert_triggered()
{
    QListWidgetItem * item = new QListWidgetItem();
    item->setIcon(QIcon(":/images/icons/13.png"));
    item->setText("Item Inserted");
    if(ui->checkBox_editable) item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt:: ItemIsUserCheckable);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->listWidget->insertItem(ui->listWidget->currentRow(), item);
}


void MainWindow::on_action_add_triggered()
{
    QListWidgetItem * item = new QListWidgetItem();
    item->setIcon(QIcon(":/images/icons/13.png"));
    item->setText("Item Added");
    if(ui->checkBox_editable) item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt:: ItemIsUserCheckable);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->listWidget->addItem(item);
}


void MainWindow::on_action_delete_triggered()
{
    int row = ui->listWidget->currentRow();
    delete ui->listWidget->takeItem(row);
}


void MainWindow::on_action_all_triggered()
{
    int count = ui->listWidget->count();
    for(int i = 0; i < count; ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
}


void MainWindow::on_action_none_triggered()
{
    int count = ui->listWidget->count();
    for(int i = 0; i < count; ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}


void MainWindow::on_action_inv_triggered()
{
    int count = ui->listWidget->count();
    for(int i = 0; i < count; ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        if(item->checkState() == Qt::Unchecked) item->setCheckState(Qt::Checked);
        else item->setCheckState(Qt::Unchecked);
    }
}

// 窗口动作：选中list item则文本行和文本框显示当前项或前一项
void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current) {
        if(previous) ui->lineEdit->setText(QString("prev: " + previous->text() + "cur: " + current->text()));
        else ui->lineEdit->setText(QString("cur: " + current->text()));
        ui->plainTextEdit->appendPlainText("signal has been sent");
    }
}

// 信号发射toolbox文本功能设置
void MainWindow::on_toolButton_txt_clear_clicked()
{
    ui->plainTextEdit->clear();
}


void MainWindow::on_toolButton_empty_line_clicked()
{
    ui->plainTextEdit->appendPlainText("");
}

// toolbutton排序list处理
void MainWindow::on_checkBox_sort_clicked(bool checked)
{
    ui->listWidget->setSortingEnabled(checked);
}


void MainWindow::on_toolButton_ascend_clicked()
{
    if(ui->checkBox_sort->isChecked())
    ui->listWidget->sortItems(Qt::AscendingOrder);
}


void MainWindow::on_toolButton_descend_clicked()
{
    if(ui->checkBox_sort->isChecked())
    ui->listWidget->sortItems(Qt::DescendingOrder);
}

// 窗口右键菜单列出动作选项
void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);      // 参数不使用
    QMenu *cursor_menu = new QMenu();       // 每次右键都new出来，不加入对象树
    cursor_menu->addAction(ui->action_all);
    cursor_menu->addAction(ui->action_delete);
    cursor_menu->addAction(ui->action_insert);
    cursor_menu->addSeparator();
    cursor_menu->addAction(ui->action_none);
    cursor_menu->addAction(ui->action_inv);
    cursor_menu->addAction(ui->action_quit);
    cursor_menu->exec(QCursor::pos());      // 跟随指针右键执行
    delete cursor_menu;
}

