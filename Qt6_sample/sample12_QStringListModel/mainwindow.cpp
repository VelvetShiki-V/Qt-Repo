#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化listview显示内容，备份至m_list中用于恢复
    m_list << "1.北京" << "2.上海" << "3.广州" << "4.福建" << "5.云南";
    m_model = new QStringListModel(this);
    m_model->setStringList(m_list);
    ui->listView->setModel(m_model);
    ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked |
                                  QAbstractItemView::SelectedClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_restore_clicked()
{
    m_model->setStringList(m_list);
}


void MainWindow::on_pushButton_clear_clicked()
{

    m_model->removeRows(0, m_model->rowCount());
}

// 从listview最后一行后插入
void MainWindow::on_pushButton_append_clicked()
{
    m_model->insertRow(m_model->rowCount());
    QModelIndex last_index = m_model->index(m_model->rowCount() - 1);      // 下标从0开始
    m_model->setData(last_index, "新添加城市", Qt::DisplayRole);     // 为指定新添加行设置属性值
    ui->listView->setCurrentIndex(last_index);
}


void MainWindow::on_pushButton_insert_clicked()
{
    QModelIndex current_index = ui->listView->currentIndex();
    m_model->insertRow(current_index.row());
    m_model->setData(current_index, "新插入城市", Qt::DisplayRole);
    ui->listView->setCurrentIndex(current_index);
}


void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked)
        ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked |
                                      QAbstractItemView::SelectedClicked);
    else
        ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MainWindow::on_pushButton_9_clicked()
{
    m_model->removeRow(ui->listView->currentIndex().row());
}


void MainWindow::on_pushButton_sort_clicked(bool checked)
{
    if(checked) m_model->sort(0, Qt::AscendingOrder);
    else m_model->sort(0, Qt::DescendingOrder);
}


void MainWindow::on_pushButton_move_up_clicked()
{
    int current_row = ui->listView->currentIndex().row();
    // 其中两个parent母参数无意义，使用匿名对象传参即可
    m_model->moveRow(QModelIndex(), current_row, QModelIndex(), current_row - 1);
    // insert在list中向前插入，向上移-1行插入，向下移动则需要+2行再插入
}


void MainWindow::on_pushButton_move_down_clicked()
{
    int current_row = ui->listView->currentIndex().row();
    m_model->moveRow(QModelIndex(), current_row, QModelIndex(), current_row + 2);
}


void MainWindow::on_pushButton_clear_txt_clicked()
{
    ui->plainTextEdit->clear();
}


void MainWindow::on_pushButton_display_clicked()
{
    ui->plainTextEdit->clear();
    QStringList list = m_model->stringList();       // 取出模型的stringlist
    QString str;
    for(int i = 0; i < list.size(); ++i) {
        str += list[i] + '\n';
    }
    ui->plainTextEdit->appendPlainText(str);
}


void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    // 单击状态栏显示
    QString display = QString::asprintf("row: %d", ui->listView->currentIndex().row());
    display += m_model->data(ui->listView->currentIndex(), Qt::DisplayRole).toString();
    ui->statusbar->showMessage(display);
}
