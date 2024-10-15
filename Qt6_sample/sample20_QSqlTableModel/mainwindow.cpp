#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->splitter);     // 使两个大的布局居中于窗口
    ui->tableView->setAlternatingRowColors(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_open_file_triggered()
{
    // 打开数据库
    DB = QSqlDatabase::addDatabase("QMYSQL");
    DB.setPort(3306);                          // MySQL数据库端口
    DB.setUserName("root");                   // 数据库用户名
    DB.setPassword("123456");                // 数据库密码
    DB.setDatabaseName("mydb");             // 数据库名称
//    DB.setHostName(serverIP);            // 数据库远程服务器IP地址
    if(!DB.open()) {
        QMessageBox::warning(this, "警告", "数据库打开失败: " + DB.lastError().text());
        return;
    }
    open_table();
}

// 处理保存和撤销功能按钮
void MainWindow::do_current_item_changed(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    Q_UNUSED(current);
    ui->action_save->setEnabled(m_model->isDirty());        // 如果model有数据变化，则保存和撤销功能启用
    ui->action_undo->setEnabled(m_model->isDirty());
}

void MainWindow::do_current_row_changed(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous);
    if(!current.isValid()) {
        ui->label_photo->clear();
        return;
    }
    ui->action_photo_clear->setEnabled(true);
    ui->action_photo_set->setEnabled(true);
    ui->action_delete_line->setEnabled(true);
    // 将当前行所有信息进行映射
    m_mapper->setCurrentIndex(current.row());

    // 设置图片
    QSqlRecord row_rec = m_model->record(current.row());
    if(row_rec.isNull("photo")) {
        ui->label_photo->clear();
        return;
    }
    // 图片渲染输出
    QPixmap pix;
    pix.loadFromData(row_rec.value("photo").toByteArray());
    ui->label_photo->setPixmap(pix.scaledToWidth(ui->label_photo->size().width()));
}

void MainWindow::open_table()
{
    m_model = new QSqlTableModel(this, DB);
    // 设置打开查看的表格，提交手段和排序
    m_model->setTable("employee");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->setSort(m_model->fieldIndex("empNum"), Qt::AscendingOrder);
    // select()将表格数据填充到model中
    if(!m_model->select())
    {
        QMessageBox::critical(this, "异常", "数据库表格打开失败: "/* + m_model->lastError().text()*/);
        return;
    }

    // 设置表格标头
    m_model->setHeaderData(m_model->fieldIndex("empNum"), Qt::Horizontal, "工号");
    m_model->setHeaderData(m_model->fieldIndex("empName"), Qt::Horizontal, "姓名");
    m_model->setHeaderData(m_model->fieldIndex("gender"), Qt::Horizontal, "性别");
    m_model->setHeaderData(m_model->fieldIndex("birth"), Qt::Horizontal, "出生日期");
    m_model->setHeaderData(m_model->fieldIndex("province"), Qt::Horizontal, "省份");
    m_model->setHeaderData(m_model->fieldIndex("empDept"), Qt::Horizontal, "部门");
    m_model->setHeaderData(m_model->fieldIndex("photo"), Qt::Horizontal, "照片");
    m_model->setHeaderData(m_model->fieldIndex("memo"), Qt::Horizontal, "备注");
    // 将打开的表格数据和格式同步到tableView的model中
    ui->tableView->setModel(m_model);
    // 设置模型的选择模型，并同步给tableView
    m_selection = new QItemSelectionModel(m_model, this);
    ui->tableView->setSelectionModel(m_selection);

    // 将照片列和备注隐藏
    ui->tableView->setColumnHidden(m_model->fieldIndex("photo"), true);
    ui->tableView->setColumnHidden(m_model->fieldIndex("memo"), true);

    // 为tableView设置代理框（自定义功能单元格）
    delegate_gender = new TComboxDelegate(this);
    QStringList genders;
    genders << "男" << "女";
    delegate_gender->set_items(genders, false);
    delegate_dept = new TComboxDelegate(this);
    QStringList depts;
    depts << "研发部" << "销售部" << "产品部" << "运维部";
    delegate_dept->set_items(depts, false);
    ui->tableView->setItemDelegateForColumn(m_model->fieldIndex("gender"), delegate_gender);
    ui->tableView->setItemDelegateForColumn(m_model->fieldIndex("empDept"), delegate_dept);

    // 将所选中行数据映射到记录
    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(m_model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_mapper->addMapping(ui->spinBox_empNum, m_model->fieldIndex("empNum"));
    m_mapper->addMapping(ui->lineEdit_name, m_model->fieldIndex("empName"));
    m_mapper->addMapping(ui->comboBox_sex, m_model->fieldIndex("gender"));
    m_mapper->addMapping(ui->comboBox_birth, m_model->fieldIndex("birth"));
    m_mapper->addMapping(ui->comboBox_nation, m_model->fieldIndex("province"));
    m_mapper->addMapping(ui->comboBox_dept, m_model->fieldIndex("empDept"));
    m_mapper->addMapping(ui->spinBox_salary, m_model->fieldIndex("salary"));
//    m_mapper->addMapping(ui->label_photo, m_model->fieldIndex("photo"));      // 图片单独使用pixmap渲染输出
    m_mapper->addMapping(ui->plainTextEdit, m_model->fieldIndex("memo"));
    m_mapper->toFirst();        // 默认将第一行表格信息填充到映射记录中
    // 当当前光标选中不同行时，填充对应信息到记录中
    connect(m_selection, &QItemSelectionModel::currentChanged, this, &MainWindow::do_current_item_changed);
    connect(m_selection, &QItemSelectionModel::currentRowChanged, this, &MainWindow::do_current_row_changed);

    // 当选中不同行时，对应功能按钮开启或关闭
    ui->action_append_line->setEnabled(true);
    ui->action_insert_line->setEnabled(true);
    ui->action_open_file->setEnabled(false);
    ui->action_salary_increase->setEnabled(true);
    ui->groupBox_info->setEnabled(true);
    ui->groupBox_order->setEnabled(true);
    ui->groupBox_filter->setEnabled(true);

    // 将标头信息填充到排序的筛选下拉框中
    QSqlRecord filter = m_model->record();
    for(int i = 0; i < filter.count(); ++i) {
        ui->comboBox_order->addItem(filter.fieldName(i));
    }
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

// 添加新行
void MainWindow::on_action_append_line_triggered()
{
    QSqlRecord rec = m_model->record();
    rec.setValue("empNum", "00000");        // 两种写法都可
    rec.setValue(m_model->fieldIndex("empName"), "NEW");
    m_model->insertRecord(m_model->rowCount(), rec);        // 在最后一行插入
    // 将选择的单元格定位到新行上
    m_selection->clearSelection();
    m_selection->setCurrentIndex(m_model->index(m_model->rowCount() - 1, 1), QItemSelectionModel::Select);
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

// 插入新行
void MainWindow::on_action_insert_line_triggered()
{
    QModelIndex index = m_selection->currentIndex();
    QSqlRecord rec = m_model->record();
    rec.setValue("empNum", "00000");        // 两种写法都可
    rec.setValue(m_model->fieldIndex("empName"), "NEW");
    m_model->insertRecord(index.row(), rec);
    m_selection->clearSelection();
    m_selection->setCurrentIndex(index, QItemSelectionModel::Select);
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

// 删除所选行
void MainWindow::on_action_delete_line_triggered()
{
    QModelIndex index = m_selection->currentIndex();
    m_model->removeRow(index.row());
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

// 为所选行设置图片
void MainWindow::on_action_photo_set_triggered()
{
    QString file_path = QFileDialog::getOpenFileName(this, "选择一个图片文件", "",
                                                    "jpg文件(*.jpg);;png文件(*.png)");
    QFile* pic = new QFile(file_path);
    if(!pic || !pic->open(QIODevice::ReadOnly)) return;
    // 将图片文件转换为record条目，并存入model
    QByteArray pic_stream = pic->readAll();
    pic->close();
    delete pic;
    QSqlRecord rec = m_model->record();
    rec.setValue(m_model->fieldIndex("photo"), pic_stream);
    m_model->setRecord(m_selection->currentIndex().row(), rec);
    // 再将图片输出到窗口
    QPixmap pix;
    pix.loadFromData(rec.value("photo").toByteArray());
    ui->label_photo->setPixmap(pix.scaledToWidth(ui->label_photo->size().width()));
}

// 为所选行清除图片
void MainWindow::on_action_photo_clear_triggered()
{
    QSqlRecord rec = m_model->record(m_selection->currentIndex().row());
    rec.setNull(m_model->fieldIndex("photo"));
    m_model->setRecord(m_selection->currentIndex().row(), rec);
    ui->label_photo->clear();
}

// 所选行工资上涨10%
void MainWindow::on_action_salary_increase_triggered()
{
    QSqlRecord rec = m_model->record(m_selection->currentIndex().row());
    rec.setValue("salary", rec.value("salary").toFloat() * 1.1);
    m_model->setRecord(m_selection->currentIndex().row(), rec);
    if(m_model->submitAll())
        QMessageBox::information(this, "消息", "操作成功");
}

// 保存
void MainWindow::on_action_save_triggered()
{
    bool ret = m_model->submitAll();
    if(!ret)
        QMessageBox::warning(this, "警告", "保存失败: " + m_model->lastError().text());
    else
    {
        ui->action_save->setEnabled(false);
        ui->action_undo->setEnabled(false);
    }
    QMessageBox::information(this, "消息", "保存成功");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

// 撤销
void MainWindow::on_action_undo_triggered()
{
    m_model->revertAll();
    ui->action_save->setEnabled(false);
    ui->action_undo->setEnabled(false);
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

// 排序功能
void MainWindow::on_comboBox_order_currentIndexChanged(int index)
{
    if(ui->radioButton_ascend->isChecked())
        m_model->sort(index, Qt::AscendingOrder);
    else
        m_model->sort(index, Qt::DescendingOrder);
    m_model->select();
}


void MainWindow::on_radioButton_ascend_clicked()
{
    m_model->setSort(ui->comboBox_order->currentIndex(), Qt::AscendingOrder);
    m_model->select();

}


void MainWindow::on_radioButton_descend_clicked()
{
    m_model->setSort(ui->comboBox_order->currentIndex(), Qt::DescendingOrder);
    m_model->select();

}

// 筛选功能
void MainWindow::on_radioButton_man_clicked()
{
    m_model->setFilter("gender = '男' ");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}


void MainWindow::on_radioButton_girl_clicked()
{
    m_model->setFilter("gender = '女' ");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}


void MainWindow::on_radioButton_all_sex_clicked()
{
    m_model->setFilter("");
    ui->statusbar->showMessage(QString("记录条数为: %1").arg(m_model->rowCount()));
}

