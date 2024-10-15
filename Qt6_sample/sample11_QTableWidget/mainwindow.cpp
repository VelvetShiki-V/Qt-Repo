#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 成员属性初始化
    cell_pos = new QLabel(this);
    cell_type = new QLabel( this);
    student_id = new QLabel(this);
    // 状态栏添加组件
    ui->statusbar->addWidget(cell_pos);
    ui->statusbar->addWidget(cell_type);
    ui->statusbar->addWidget(student_id);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_header_clicked()
{
    QStringList headers;
    headers << "姓名" << "性别" << "出生日期" << "民族" << "分数" << "是否党员";
    ui->tableWidget->setColumnCount(headers.count());
    for(int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        QTableWidgetItem* header = new QTableWidgetItem();
        QFont font;
        font.setBold(true);
        font.setPointSize(11);
        header->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        header->setFont(font);
        header->setForeground(QBrush(Qt::red));
        header->setText(headers[i]);
        // 水平表头循环依次插入
        ui->tableWidget->setHorizontalHeaderItem(i, header);
    }
}


void MainWindow::on_pushButton_row_set_clicked()
{
    ui->tableWidget->setRowCount(ui->spinBox->value());
    for(int i = 1; i <= ui->tableWidget->rowCount(); ++i) {
        QTableWidgetItem* header = new QTableWidgetItem();
        header->setText(QString::number(i));
        // 垂直表头循环依次插入
        ui->tableWidget->setVerticalHeaderItem(i, header);
    }
}


void MainWindow::on_pushButton_init_clicked()
{
    ui->tableWidget->clear();
    ui->pushButton_header->click();
    ui->tableWidget->setRowCount(10);
    QDate birth(2000, 1, 1);
    QString ethnic = "汉族";
    for(int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString sex = (row % 2) == 0 ? "男" : "女";
        short grade = QRandomGenerator::global()->bounded(60, 100);
        bool pm = (row % 2) == 0 ? true : false;
        info_registration_to_one_row(row, QString("学生%1").arg(row + 1), sex, birth, ethnic, grade, pm);
        birth = birth.addDays(20);
    }
}

void MainWindow::info_registration_to_one_row(int row, const QString &name, const QString &sex, const QDate &date, const QString &ethnic, const short &grade, bool pm)
{
    QTableWidgetItem* item;
    int stu_id = 10010 + row;
    // 姓名
    item = new QTableWidgetItem(name, CTname);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setData(Qt::UserRole, stu_id);        // 用户值插入（学生学号——状态栏显示）
    ui->tableWidget->setItem(row, colName, item);
    // 性别
    item = new QTableWidgetItem(sex, CTsex);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if(sex == "男") item->setIcon(QIcon(":/images/icons/1.png"));
    else item->setIcon(QIcon(":/images/icons/2.png"));
    ui->tableWidget->setItem(row, colSex, item);
    // 出生日期
    item = new QTableWidgetItem(date.toString("yyyy-MM-dd"), CTdate);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, colDate, item);
    // 民族
    item = new QTableWidgetItem(ethnic, CTethnic);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, colEthnic, item);
    // 分数
    item = new QTableWidgetItem(QString::number(grade), CTgrade);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->setItem(row, colGrade, item);
    // 党员
    item = new QTableWidgetItem("党员", CTpm);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    item->setBackground(QBrush(Qt::yellow));
    if(pm) item->setCheckState(Qt::Checked);
    else item->setCheckState(Qt::Unchecked);
    ui->tableWidget->setItem(row, colPM, item);
}


void MainWindow::on_pushButton_row_insert_clicked()
{
    int row = ui->tableWidget->currentRow();
    ui->tableWidget->insertRow(row);
    info_registration_to_one_row(row, "新学生", "男", QDate(2000,1,1), "汉族", 100, true);
}


void MainWindow::on_pushButton_row_append_clicked()
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    info_registration_to_one_row(row, "新学生", "男", QDate(2000,1,1), "汉族", 100, true);
}


void MainWindow::on_pushButton_row_delete_clicked()
{
    int row = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(row);
}


void MainWindow::on_pushButton_row_adjust_clicked()
{
    ui->tableWidget->resizeRowsToContents();
}


void MainWindow::on_pushButton_column_adjust_clicked()
{
    ui->tableWidget->resizeColumnsToContents();
}


void MainWindow::on_pushButton_read_clicked()
{
    ui->plainTextEdit->clear();
    for(int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString content = QString::asprintf("第%d号学生: ", row + 1);
        for(int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QString str = ui->tableWidget->item(row, col)->text();
            if(col == colPM) {
                if(!ui->tableWidget->item(row, col)->checkState())
                    str = "非党员";
            }
            content += str + " ";
        }
        ui->plainTextEdit->appendPlainText(content);
    }
}


void MainWindow::on_checkBox_editable_clicked(bool checked)
{
    if(checked) {
        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);;
    }
    else {
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    ui->pushButton_row_insert->setEnabled(checked);
    ui->pushButton_row_append->setEnabled(checked);
    ui->pushButton_row_delete->setEnabled(checked);
    ui->pushButton_column_adjust->setEnabled(checked);
    ui->pushButton_row_adjust->setEnabled(checked);
    ui->pushButton_row_set->setEnabled(checked);
    ui->pushButton_init->setEnabled(checked);
}


void MainWindow::on_checkBox_color_clicked(bool checked)
{
    ui->tableWidget->setAlternatingRowColors(checked);
}


void MainWindow::on_checkBox_header_clicked(bool checked)
{
    ui->tableWidget->horizontalHeader()->setVisible(checked);
}


void MainWindow::on_checkBox_vHeader_clicked(bool checked)
{
    ui->tableWidget->verticalHeader()->setVisible(checked);
}


void MainWindow::on_radioButton_row_select_clicked()
{
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}


void MainWindow::on_radioButton_item_select_clicked()
{
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
}


void MainWindow::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(previousRow);
    Q_UNUSED(previousColumn);
    cell_pos->setText(QString("单元格坐标: row: %1, col: %2").arg(currentRow).arg(currentColumn));
    cell_type->setText("单元格类型: " + QString::number(ui->tableWidget->currentItem()->type()));
    if(currentColumn == colName)
    student_id->setText("学生学号: " + QString::number(ui->tableWidget->currentItem()->data(Qt::UserRole).toInt()));
}

