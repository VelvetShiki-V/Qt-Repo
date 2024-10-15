#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tdialogaccessdata.h"
#include "tdialogdomaindata.h"
#include "tdialogemployeedata.h"
#include "tdialogroledata.h"
#include "tdialogrbacdata.h"
#include "tdialoguserdata.h"
#include "tdialoglogin.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 设置状态栏
    database_name = new QLabel("打开的数据库", this);
    row_locate = new QLabel("当前行", this);
    record_count = new QLabel("记录条数", this);
    table_name = new QLabel("打开的表名", this);
    table_name->setMinimumWidth(200);
    database_name->setMinimumWidth(200);
    row_locate->setMinimumWidth(200);
    record_count->setMinimumWidth(200);
    ui->statusbar->addWidget(database_name);
    ui->statusbar->addWidget(table_name);
    ui->statusbar->addWidget(row_locate);
    ui->statusbar->addWidget(record_count);
    // treeView项属性初始化
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    // 表格属性初始化
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    // 表格的右键菜单初始化和策略（必要）
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 当运行程序时的弹出窗口
void MainWindow::showEvent(QShowEvent *event)
{
    if(first_time_show) {
        TDialogLogin* login = new TDialogLogin();
        if(login->exec() == QDialog::Accepted) {
            // 打开数据库（设置驱动，用户名密码，ip端口号，和默认需要打开的数据库名称）
            DB = QSqlDatabase::addDatabase("QMYSQL");
            DB.setUserName(login->user());
            DB.setPassword(login->password());
            DB.setHostName(login->ip());
            DB.setPort(3306);
            DB.setDatabaseName(default_open_database);         // 数据库打开前需要设置数据库名称
            // 登录成功时需要进行身份核验，否则登陆失败
            if(!DB.open()) {
                QMessageBox::critical(this, "错误", "数据库打开失败: " + DB.lastError().text());
                showEvent(event);
            }
            else {
                if(user_authenticate()) {
                    // 身份验证成功，界面treeView将列出所有的可查看数据库名称
                    memory_leak_check_release();
                    list_all_databases();
                }
                else
                   showEvent(event);
            }
            event->accept();
            first_time_show = false;
        }
        else {
            event->ignore();
//            MainWindow::close();
        }
    }
}

// 当点击表格内容时，启用特定功能按钮，并定位所在行
void MainWindow::on_tableView_clicked(const QModelIndex& index)
{
    Q_UNUSED(index);
    ui->action_delete->setEnabled(true);
    ui->action_edit->setEnabled(true);
    row_locate->setText(QString("当前所在行: %1").arg(index.row() + 1));
}

// 点击列表框时，将根据点击的不同项显示不同内容（父项数据库展示子项表格，子项表格展示表内容）
void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    // 根据点击的为父项或子项执行不同功能
    QVariant item_data = index.data();
    if(!index.parent().isValid()) {
        // 点击的是父项（数据库名），打开这个数据库，并将其包含的所有表名列出来
        if(item_data.canConvert<QString>()) {
            QString db_name = item_data.toString();
            // 需要再单独设置数据库名称并打开
            DB.setDatabaseName(db_name);
            if(!DB.open() || !DB.isValid()) {
                QMessageBox::critical(this, "错误", "数据库打开失败: " + DB.lastError().text());
                return;
            }
            // 打开其他表前，如果已存在打开的表名，则清空它们，再展示旗下所有表
            if(table_list)
                table_list->removeRows(0, table_list->rowCount());
            list_tables(index);
        }
        database_name->setText("打开的数据库名: " + DB.databaseName());
    }
    else {
        // 点击的是子项（表名），将该表的数据通过model展示
        // 内存泄漏处理
        if(sql_model) delete sql_model;
        if(relation_model) delete relation_model;
        if(m_selection) delete m_selection;
        sql_model = nullptr;
        m_selection = nullptr;
        relation_model = nullptr;
        if(item_data.canConvert<QString>())
            current_table_name = item_data.toString();
        // 根据表是否是外键关联表选择不同的模型，并通过模型展示表格数据
        if(is_relational())
            show_relational_table();
        else
            select_data();
        // 设置treeView显示的项数据样式
        QFont font;
        font.setPointSize(15);
        ui->tableView->setFont(font);
        // 点击treeView按钮功能的默认状态
        ui->action_append->setEnabled(true);
        ui->action_delete->setEnabled(false);
        ui->action_edit->setEnabled(false);
        ui->action_first->setEnabled(false);
        ui->action_last->setEnabled(false);
        ui->action_next->setEnabled(false);
        ui->action_previous->setEnabled(false);
        // 点击tableView按钮功能的状态
        connect(ui->tableView, &QTableView::clicked, this, &MainWindow::on_tableView_clicked);
        connect(ui->tableView, &QTableView::clicked, this, &MainWindow::on_tableItem_move);
        table_name->setText("打开的表名: " + current_table_name);
        // 默认选中首行内容（伪选择）
        select_row(0);
    }
}

// 主表（非外键关联表）选择的QSqlQueryModel模型表格数据展示
void MainWindow::select_data()
{
    sql_model = new QSqlQueryModel(this);
    m_selection = new QItemSelectionModel(sql_model, this);
    // 查询该表的所有条目
    QString query = "select * from " + current_table_name;      // **表名，列名等不能使用绑定，只能拼接**
    sql_model->setQuery(query);
    if(sql_model->lastError().isValid()) {
        QMessageBox::critical(this, "错误", "表数据查询失败: " + sql_model->lastError().text());
        return;
    }
    // 在将模型设置到表上前，进行表头，列隐藏等细节处理
    header_handle();
    // 将模型加载到UI控件上
    ui->tableView->setModel(sql_model);
    ui->tableView->setSelectionModel(m_selection);
    m_selection->setCurrentIndex(sql_model->index(0, 0), QItemSelectionModel::ClearAndSelect);
    record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
}

// 对特定表进行标头处理
void MainWindow::header_handle()
{
    if(current_table_name == "employee") {
        QSqlRecord rec = sql_model->record();
        ui->tableView->setColumnHidden(7, true);
        ui->tableView->setColumnHidden(8, true);
        sql_model->setHeaderData(rec.indexOf("empNum"), Qt::Horizontal, "工号");
        sql_model->setHeaderData(rec.indexOf("empName"), Qt::Horizontal, "名字");
        sql_model->setHeaderData(rec.indexOf("gender"), Qt::Horizontal, "性别");
        sql_model->setHeaderData(rec.indexOf("birth"), Qt::Horizontal, "出生日期");
        sql_model->setHeaderData(rec.indexOf("province"), Qt::Horizontal, "省份");
        sql_model->setHeaderData(rec.indexOf("empDept"), Qt::Horizontal, "部门");
        sql_model->setHeaderData(rec.indexOf("salary"), Qt::Horizontal, "工资");
    }
    else if(current_table_name == "role") {
        QSqlRecord rec = sql_model->record();
        sql_model->setHeaderData(rec.indexOf("name"), Qt::Horizontal, "名字");
        sql_model->setHeaderData(rec.indexOf("memo"), Qt::Horizontal, "备注");
    }
    else if(current_table_name == "access") {
        QSqlRecord rec = sql_model->record();
        sql_model->setHeaderData(rec.indexOf("name"), Qt::Horizontal, "名字");
        sql_model->setHeaderData(rec.indexOf("memo"), Qt::Horizontal, "备注");
    }
    else if(current_table_name == "domain") {
        relation_model->setHeaderData(relation_model->fieldIndex("name"), Qt::Horizontal, "名称");
        relation_model->setHeaderData(relation_model->fieldIndex("super"), Qt::Horizontal, "父域");
        relation_model->setHeaderData(relation_model->fieldIndex("memo"), Qt::Horizontal, "备注");
    }
    else if(current_table_name == "rbac") {
        relation_model->setHeaderData(relation_model->fieldIndex("role"), Qt::Horizontal, "角色");
        relation_model->setHeaderData(relation_model->fieldIndex("access"), Qt::Horizontal, "访问");
        relation_model->setHeaderData(relation_model->fieldIndex("domain"), Qt::Horizontal, "数据域");
    }
    else if(current_table_name == "user") {
        relation_model->setHeaderData(relation_model->fieldIndex("name"), Qt::Horizontal, "名称");
        relation_model->setHeaderData(relation_model->fieldIndex("password"), Qt::Horizontal, "口令");
        relation_model->setHeaderData(relation_model->fieldIndex("role"), Qt::Horizontal, "角色");
        relation_model->setHeaderData(relation_model->fieldIndex("createtime"), Qt::Horizontal, "创建时间");
        relation_model->setHeaderData(relation_model->fieldIndex("expiration"), Qt::Horizontal, "终止期限");
        relation_model->setHeaderData(relation_model->fieldIndex("company"), Qt::Horizontal, "单位");
        relation_model->setHeaderData(relation_model->fieldIndex("mobile"), Qt::Horizontal, "电话");
        relation_model->setHeaderData(relation_model->fieldIndex("memo"), Qt::Horizontal, "备注");
    }
}

// 查看该表是否为关联外键表
bool MainWindow::is_relational()
{
    QStringList relational;
    relational << "rbac" << "user" << "domain";
    return relational.contains(current_table_name);
}

// 外键关联表选择的QSqlRelationalTableModel展示数据，为QSqlQueryModel和QSqlTable的派生类
void MainWindow::show_relational_table()
{
    // 关联表模型初始化操作
    relation_model = new QSqlRelationalTableModel(this, DB);
    relation_model->setTable(current_table_name);
    m_selection = new QItemSelectionModel(relation_model, this);
    // 标头替换
    header_handle();
    // 将外键字段与其他表关联
    set_relation();

//    **QSqlRelationalTableModel在使用select语句后会将外键所在列字段名fieldname添加修饰以提高效率**
//    qDebug() << "\nfieldname TEST: before select";
//    for(int i = 0; i < relation_model->record().count(); ++i) {
//        qDebug() << relation_model->record().fieldName(i) << ":" << relation_model->record().value(i);
//    }

    // 渲染表格输出
    if(!relation_model->select()) {
        QMessageBox::critical(this, "错误", "外键表数据查询失败: " + relation_model->lastError().text());
        return;
    }

//    qDebug() << "\nfieldname TEST: after select";
//    for(int i = 0; i < relation_model->record().count(); ++i) {
//        qDebug() << relation_model->record().fieldName(i) << ":" << relation_model->record().value(i);
//    }

    // 初始化外键关联表relationalTable的模型和selection选择策略
    ui->tableView->setModel(relation_model);
    ui->tableView->setSelectionModel(m_selection);
    m_selection->setCurrentIndex(relation_model->index(0, 0), QItemSelectionModel::ClearAndSelect);
    record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
}

void MainWindow::set_relation()
{
    if(current_table_name == "rbac") {
//      setRelation格式：[column][tableName][indexColumn][displayColumn]
//      column: 要设置关联关系的列的索引,列索引从0开始
//      tableName：关联表的名称
//      indexColumn：关联表中用作索引的列的名称。该列通常是一个主键或唯一键
//      displayColumn：关联表中用于显示的列的名称。这是在模型中显示关联数据时使用的列
//      **该函数可能会影响到外键关联主表时，select后会潜在地变更外键所在列字段名field name**
        relation_model->setRelation(relation_model->fieldIndex("role"),
                                    QSqlRelation("role", "name", "name"));
        relation_model->setRelation(relation_model->fieldIndex("access"),
                                    QSqlRelation("access", "name", "name"));
        relation_model->setRelation(relation_model->fieldIndex("domain"),
                                    QSqlRelation("domain", "name", "name"));
    }
    else if(current_table_name == "user") {
        relation_model->setRelation(relation_model->fieldIndex("role"),
                                    QSqlRelation("role", "name", "name"));
    }
    else if(current_table_name == "domain") {
        relation_model->setRelation(relation_model->fieldIndex("super"),
                                    QSqlRelation("domain", "name", "name"));
    }
}

// 对表格CRUD时会自动将光标选择的单元格高光定位到最新操作的行
void MainWindow::newest_locate(QAbstractTableModel *abs_model, const QSqlRecord& record, const int& model_flag)
{
    // 将模型具体化
    int row;
    QModelIndex cur_index;
    if(model_flag == query_model) {
        // 操作的表为主表query模型时，查询其当前表的对应记录行，并将selectionModel更新至对应index上
        QSqlQueryModel* model = dynamic_cast<QSqlQueryModel*>(abs_model);
        for(row = 0; row < model->rowCount(); ++row) {
            if(model->record(row) == record) break;
        }
        cur_index = model->index(row, 0);
    }
    else if(model_flag == relational_model) {
        // 操作的表为外键关联表relational模型，执行相同逻辑
        QSqlRelationalTableModel* model = dynamic_cast<QSqlRelationalTableModel*>(abs_model);
        for(row = 0; row < model->rowCount(); ++row) {
            if(model->record(row) == record) break;
        }
        cur_index = model->index(row, 0);
    }
    // 选中行前先设置索引
    m_selection->setCurrentIndex(cur_index, QItemSelectionModel::ClearAndSelect);
    // 将新插入或更新后的行选中，以方便观察
    select_row(cur_index.row());
}

void MainWindow::select_row(const int& row)
{
    // 构造需要选中的区域（topLeft->bottomRight）
    QModelIndex start_index = m_selection->currentIndex().sibling(row, 0);
    QModelIndex end_index = m_selection->currentIndex().sibling(row, m_selection->model()->columnCount() - 1);
    QItemSelection selection(start_index, end_index);
    // 清空并将整行选中
    m_selection->select(selection, QItemSelectionModel::ClearAndSelect);
    // 记录状态栏行数状态
    row_locate->setText(QString("当前所在行: %1").arg(m_selection->currentIndex().row() + 1));
}

void MainWindow::memory_leak_check_release()
{
    // 如果打开新连接前存在已打开的数据库，将其关闭
    if(sql_model) delete sql_model;
    if(table_list) {
        table_list->removeRows(0, table_list->rowCount());
        delete table_list;
    }
    if(m_selection) delete m_selection;
    if(database_model) delete database_model;
    if(relation_model) delete relation_model;
    table_list = nullptr;
    sql_model = nullptr;
    m_selection = nullptr;
    database_model = nullptr;
    relation_model = nullptr;
}

// 密码的sha256哈希加密算法
QString MainWindow::calculate_sha256(const QString &password)
{
    // 将密码字符串转为utf8字节流，确保在哈希计算过程中使用正确的字节表示
    QByteArray pwd_stream = password.toUtf8();
    QByteArray hash_data = QCryptographicHash::hash(pwd_stream, QCryptographicHash::Sha256);
    // 将加密结果的二进制字节流数据转成16进制，得到一个更短、可读性更好的表示形式
    QString transfered_pwd = QString(hash_data.toHex());
    return transfered_pwd;
}

// 数据库添加数据
void MainWindow::on_action_append_triggered()
{
    if(current_table_name == "employee") {
        TDialogEmployeeData* emp = new TDialogEmployeeData();
        // 初始化条目格式（必要）
        QSqlQuery query;
        query.exec("select * from employee where empNum = -1");
        QSqlRecord init_record = query.record();
        // 同步的条目格式字段，**必须与数据库中的字段名保持一致**
        emp->set_record(init_record);       // 将条目格式同步给dialog的QSqlQuery
        if(emp->exec() == QDialog::Accepted) {
            // 接收对话框中的条目信息
            QSqlRecord ret = emp->get_record();
            // 条目接收完毕，将获取到的条目执行sql语句，同步到数据库中
            QSqlQuery query;
            query.prepare("insert into employee (empNum, empName, gender, birth, province, "
                            "empDept, salary, photo, memo) values (:NUM, :NAME, :GENDER, :BIRTH,"
                          " :PROVINCE, :DEPT, :SALARY, :PHOTO, :MEMO)");
            query.bindValue(":NUM", ret.value("empNum"));
            query.bindValue(":NAME", ret.value("empName"));
            query.bindValue(":GENDER", ret.value("gender"));
            query.bindValue(":BIRTH", ret.value("birth"));
            query.bindValue(":PROVINCE", ret.value("province"));
            query.bindValue(":DEPT", ret.value("empDept"));
            query.bindValue(":SALARY", ret.value("salary"));
            query.bindValue(":PHOTO", ret.value("photo"));
            query.bindValue(":MEMO", ret.value("memo"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "数据库添加条目失败: " + query.lastError().text());
            else {
                // 刷新当前tableView的QSqlQueryModel
                sql_model->setQuery(sql_model->query().executedQuery());
                newest_locate(sql_model, ret);
            }
        }
        delete emp;
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "role") {
        TDialogRoleData* role = new TDialogRoleData();
        QSqlRecord init_record = sql_model->record();
        role->set_record(init_record);
        if(role->exec() == QDialog::Accepted) {
            QSqlRecord ret = role->get_record();
            QSqlQuery query;
            query.prepare("insert into role (name, memo) values (:NAME, :MEMO)");
            query.bindValue(":NAME", ret.value("name"));
            query.bindValue(":MEMO", ret.value("memo"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "role表数据添加失败: " + query.lastError().text());
            else
            {
                sql_model->setQuery(sql_model->query().executedQuery());
                newest_locate(sql_model, ret);
            }
        }
        delete role;
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "access") {
        TDialogAccessData* access = new TDialogAccessData();
        QSqlRecord rec = sql_model->record();
        access->set_record(rec);
        if(access->exec() == QDialog::Accepted) {
            QSqlRecord ret = access->get_record();
            QSqlQuery query;
            query.prepare("insert into access (name, memo) values (:NAME, :MEMO)");
            query.bindValue(":NAME", ret.value("name"));
            query.bindValue(":MEMO", ret.value("memo"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "access表数据添加失败: " + query.lastError().text());
            else {
                sql_model->setQuery(sql_model->query().executedQuery());
                newest_locate(sql_model, ret);
            }
        }
        delete access;
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "domain") {
        TDialogDomainData* domain = new TDialogDomainData();
        domain->set_record(relation_model, current_row);
        if(domain->exec() == QDialog::Accepted) {
            QSqlRecord ret = domain->get_record();
            if(!relation_model->insertRecord(relation_model->rowCount() - 1, ret))
                QMessageBox::critical(this, "错误", "domain表数据添加失败: " + relation_model->lastError().text());
            else {
                // 数据添加成功，对该外键关联表进行渲染输出
                relation_model->select();
                // 将当前选择行调整到新插入行上
                newest_locate(relation_model, ret);
            }
        }
        delete domain;
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else if(current_table_name == "rbac") {
        TDialogRbacData* rbac = new TDialogRbacData();
        rbac->set_record(relation_model, current_row);
        if(rbac->exec() == QDialog::Accepted) {
            QSqlRecord ret = rbac->get_record();
            // 对于多主键，多外键的复杂表不建议使用relationalModel自带的数据处理接口（如setRecord, insertRecord）
            // 可能会出现意想不到的问题，建议使用qsqlquery的sql语句或借助基类qsqlquerymodel并setquery来完成表格数据的处理
            QSqlQuery query(DB);
            query.prepare("insert into rbac (role, access, domain) values (:ROLE, :ACCESS, :DOMAIN)");
            query.bindValue(":ROLE", ret.value("role_name_3"));
            query.bindValue(":ACCESS", ret.value("access_name_2"));
            query.bindValue(":DOMAIN", ret.value("name"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "rbac表数据添加失败: " + query.lastError().text());
            // 刷新当前表页面
            else {
                relation_model->select();
                newest_locate(relation_model, ret);
            }
        }
        delete rbac;
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else if(current_table_name == "user") {
        TDialogUserData* user = new TDialogUserData();
        user->set_record(relation_model, current_row);
        if(user->exec() == QDialog::Accepted) {
            QSqlRecord ret = user->get_record();
            if(!relation_model->insertRecord(relation_model->rowCount() - 1, ret))
                QMessageBox::critical(this, "错误", "user表数据添加失败: " + relation_model->lastError().text());
            else {
                relation_model->select();
                newest_locate(relation_model, ret);
            }
        }
        delete user;
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else {
        // 不弹出对话框，直接在界面添加或使用代理键入值（未实现）
    }
}

// 数据库更新数据
void MainWindow::on_action_edit_triggered()
{
    if(current_table_name == "employee") {
        TDialogEmployeeData* emp = new TDialogEmployeeData();
        // 初始化条目格式（必要）
        int current_row = m_selection->currentIndex().row();
        QSqlRecord init_record = sql_model->record(current_row);
        int pm_key = init_record.value("empNum").toInt();
        QSqlQuery query;
        query.prepare("select * from employee where empNum = :OLD_NUM");
        query.bindValue(":OLD_NUM", pm_key);        // 若后续query需要该值，若重新prepare，需要重新绑定**
        query.exec();
        query.first();
        if(!query.isValid()) {
            QMessageBox::warning(this, "警告", "请选择一个条目进行编辑");
            return;
        }
        init_record = query.record();
        // 同步的条目格式字段，**必须与数据库中的字段名保持一致**
        emp->set_record(init_record);       // 将条目格式同步给dialog的QSqlQuery
        if(emp->exec() == QDialog::Accepted) {
            // 接收对话框中的条目信息
            QSqlRecord ret = emp->get_record();
            // 条目接收完毕，将获取到的条目执行sql语句，同步到数据库中
            QSqlQuery query;
            query.prepare("update employee set empNum = :NEW_NUM, empName = :NAME, gender = :GENDER,"
                          "birth = :BIRTH, province = :PROVINCE, empDept = :DEPT, salary = :SALARY,"
                          "photo = :PHOTO, memo = :MEMO where empNum = :OLD_NUM");
            query.bindValue(":NEW_NUM", ret.value("empNum"));
            query.bindValue(":OLD_NUM", pm_key);
            query.bindValue(":NAME", ret.value("empName"));
            query.bindValue(":GENDER", ret.value("gender"));
            query.bindValue(":BIRTH", ret.value("birth"));
            query.bindValue(":PROVINCE", ret.value("province"));
            query.bindValue(":DEPT", ret.value("empDept"));
            query.bindValue(":SALARY", ret.value("salary"));
            query.bindValue(":PHOTO", ret.value("photo"));
            query.bindValue(":MEMO", ret.value("memo"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "employee表数据更新失败: " + query.lastError().text());
            else
            {
                // 刷新当前tableView的QSqlQueryModel
                sql_model->setQuery(sql_model->query().executedQuery());
                newest_locate(sql_model, ret, query_model);
            }
        }
        delete emp;
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "role") {
        TDialogRoleData* role = new TDialogRoleData();
        int current_row = m_selection->currentIndex().row();
        QSqlRecord init_record = sql_model->record(current_row);
        QString pm_key = init_record.value("name").toString();
        QSqlQuery query;
        query.prepare("select * from role where name = :OLD_NAME");
        query.bindValue(":OLD_NAME", pm_key);
        query.exec();
        query.first();
        if(!query.isValid()) {
            QMessageBox::warning(this, "警告", "请选择一个条目进行编辑");
            return;
        }
        // qDebug() << "查询语句: " << query.executedQuery();
        init_record = query.record();
        role->set_record(init_record);
        if(role->exec() == QDialog::Accepted) {
            QSqlRecord ret = role->get_record();
            query.prepare("update role set name = :NEW_NAME, memo = :MEMO where name = :OLD_NAME");
            query.bindValue(":NEW_NAME", ret.value("name"));
            query.bindValue(":OLD_NAME", pm_key);
            query.bindValue(":MEMO", ret.value("memo"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "role表数据更新失败: " + query.lastError().text());
            else {
                sql_model->setQuery(sql_model->query().executedQuery());
                newest_locate(sql_model, ret, query_model);
            }
        }
        delete role;
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "access") {
        TDialogAccessData* access = new TDialogAccessData();
        int current_row = m_selection->currentIndex().row();
        QSqlRecord init_record = sql_model->record(current_row);
        QString pm_key = init_record.value("name").toString();
        QSqlQuery query;
        query.prepare("select * from access where name = :OLD_NAME");
        query.bindValue(":OLD_NAME", pm_key);
        query.exec();
        query.first();
        if(!query.isValid()) {
            QMessageBox::warning(this, "警告", "请选择一个条目进行编辑");
            return;
        }
        init_record = query.record();
        access->set_record(init_record);
        if(access->exec() == QDialog::Accepted) {
            QSqlRecord ret = access->get_record();
            query.prepare("update access set name = :NEW_NAME, memo = :MEMO where name = :OLD_NAME");
            query.bindValue(":NEW_NAME", ret.value("name"));
            query.bindValue(":OLD_NAME", pm_key);
            query.bindValue(":MEMO", ret.value("memo"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "access表数据更新失败: " + query.lastError().text());
            else {
                sql_model->setQuery(sql_model->query().executedQuery());
                newest_locate(sql_model, ret, query_model);
            }
        }
        delete access;
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "domain") {
        TDialogDomainData* domain = new TDialogDomainData();
        QModelIndex cur_index = m_selection->currentIndex();
        int this_row = cur_index.row();
        QSqlRecord init_record = relation_model->record(this_row);
        if(init_record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择一个条目进行编辑");
            return;
        }
        domain->set_record(relation_model, this_row);
        if(domain->exec() == QDialog::Accepted) {
            QSqlRecord ret = domain->get_record();
            if(!relation_model->setRecord(this_row, ret))
                QMessageBox::critical(this, "错误", "domain表数据更新失败: " + relation_model->lastError().text());
            else {
                relation_model->submitAll();
                relation_model->select();
                newest_locate(relation_model, ret, relational_model);
            }
        }
        delete domain;
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else if(current_table_name == "rbac") {
        TDialogRbacData* rbac = new TDialogRbacData();
        QModelIndex cur_index = m_selection->currentIndex();
        int this_row = cur_index.row();
        QSqlRecord init_record = relation_model->record(this_row);
        if(init_record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择一个条目进行编辑");
            return;
        }
        rbac->set_record(relation_model, this_row);
        if(rbac->exec() == QDialog::Accepted) {
            QSqlRecord ret = rbac->get_record();
            QSqlQuery query(DB);
            query.prepare("update rbac set role = :NEW_ROLE, access = :NEW_ACCESS, domain = :NEW_DOMAIN "
                          "where role = :OLD_ROLE AND access = :OLD_ACCESS AND domain = :OLD_DOMAIN");
            query.bindValue(":NEW_ROLE", ret.value("role_name_3"));
            query.bindValue(":NEW_ACCESS", ret.value("access_name_2"));
            query.bindValue(":NEW_DOMAIN", ret.value("name"));
            query.bindValue(":OLD_ROLE", init_record.value("role_name_3"));
            query.bindValue(":OLD_ACCESS", init_record.value("access_name_2"));
            query.bindValue(":OLD_DOMAIN", init_record.value("name"));
            if(!query.exec())
                QMessageBox::critical(this, "错误", "rbac表数据更新失败: " + query.lastError().text());
            else {
                relation_model->select();
                newest_locate(relation_model, ret, relational_model);
            }
        }
        delete rbac;
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else if(current_table_name == "user") {
        TDialogUserData* user = new TDialogUserData();
        QModelIndex cur_index = m_selection->currentIndex();
        int this_row = cur_index.row();
        QSqlRecord init_record = relation_model->record(this_row);
        if(init_record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择一个条目进行编辑");
            return;
        }
        user->set_record(relation_model, this_row);
        if(user->exec() == QDialog::Accepted) {
            QSqlRecord ret = user->get_record();
            if(!relation_model->setRecord(this_row, ret))
                QMessageBox::critical(this, "错误", "user表数据更新失败: " + relation_model->lastError().text());
            else {
                relation_model->submitAll();
                relation_model->select();
                newest_locate(relation_model, ret, relational_model);
            }
        }
        delete user;
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else {
        // 不弹出对话框，直接在界面修改值
    }
}

// 数据库删除数据
void MainWindow::on_action_delete_triggered()
{
    int this_row = m_selection->currentIndex().row();
    if(!m_selection->currentIndex().isValid()) {
        QMessageBox::warning(this, "警告", "请选择一个条目进行删除");
        return;
    }
    if(current_table_name == "employee") {
        QSqlRecord record = sql_model->record(this_row);
        if(record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择有效条目进行删除");
            return;
        }
        QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要删除吗？",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(ret == QMessageBox::Yes) {
            int pm_key = record.value("empNum").toInt();
            QSqlQuery query;
            query.prepare("delete from employee where empNum = :NUM");
            query.bindValue(":NUM", pm_key);
            if(!query.exec()) {
                QMessageBox::critical(this, "错误", "条目删除失败: " + query.lastError().text());
                return;
            }
            else {
                sql_model->setQuery(sql_model->query().executedQuery());
                select_row(this_row);
            }
        }
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "role") {
        QSqlRecord record = sql_model->record(this_row);
        if(record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择有效条目进行删除");
            return;
        }
        QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要删除吗？",
                                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(ret == QMessageBox::Yes) {
            QString pm_key = record.value("name").toString();
            QSqlQuery query;
            query.prepare("delete from role where name = :NAME");
            query.bindValue(":NAME", pm_key);
            if(!query.exec()) {
                QMessageBox::critical(this, "错误", "条目删除失败: " + query.lastError().text());
                return;
            }
            else {
                sql_model->setQuery(sql_model->query().executedQuery());
                select_row(this_row);
            }
        }
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "access") {
        QSqlRecord record = sql_model->record(this_row);
        if(record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择有效条目进行删除");
            return;
        }
        QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要删除吗？",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(ret == QMessageBox::Yes) {
            QString pm_key = record.value("name").toString();
            QSqlQuery query;
            query.prepare("delete from access where name = :NAME");
            query.bindValue(":NAME", pm_key);
            if(!query.exec()) {
                QMessageBox::critical(this, "错误", "条目删除失败: " + query.lastError().text());
                return;
            }
            else {
                sql_model->setQuery(sql_model->query().executedQuery());
                select_row(this_row);
            }
        }
        record_count->setText(QString("当前表记录条数: %1").arg(sql_model->rowCount()));
    }
    else if(current_table_name == "domain") {
        QSqlRecord record = relation_model->record(this_row);
        if(record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择有效条目进行删除: "  + relation_model->lastError().text());
            return;
        }
        QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要删除吗？",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(ret == QMessageBox::Yes) {
            if(!relation_model->removeRow(this_row)) {
                QMessageBox::critical(this, "错误", "条目删除失败: " + relation_model->lastError().text());
                relation_model->select();
                return;
            }
            else {
                relation_model->submitAll();
                relation_model->select();
                m_selection->setCurrentIndex(relation_model->index(this_row, 0), QItemSelectionModel::ClearAndSelect);
                select_row(this_row);
            }
        }
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else if(current_table_name == "rbac") {
        relation_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        QSqlRecord record = relation_model->record(this_row);
        if(record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择有效条目进行删除: "  + relation_model->lastError().text());
            return;
        }
        QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要删除吗？",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(ret == QMessageBox::Yes) {
            // sql语句
            QSqlQuery query(DB);
            query.prepare("delete from rbac where role = :ROLE AND access = :ACCESS AND domain = :DOMAIN");
            query.bindValue(":ROLE", record.value("role_name_3"));
            query.bindValue(":ACCESS", record.value("access_name_2"));
            query.bindValue(":DOMAIN", record.value("name"));
            if(!query.exec()) {
                QMessageBox::critical(this, "错误", "条目删除失败: " + query.lastError().text());
                return;
            }
            else {
                relation_model->select();
                m_selection->setCurrentIndex(relation_model->index(this_row, 0), QItemSelectionModel::ClearAndSelect);
                select_row(this_row);
            }
        }
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
    else if(current_table_name == "user") {
        QSqlRecord record = relation_model->record(this_row);
        if(record.isEmpty()) {
            QMessageBox::warning(this, "警告", "请选择有效条目进行删除: "  + relation_model->lastError().text());
            return;
        }
        QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要删除吗？",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(ret == QMessageBox::Yes) {
            if(!relation_model->removeRow(this_row)) {
                QMessageBox::critical(this, "错误", "条目删除失败: " + relation_model->lastError().text());
                return;
            }
            else {
                relation_model->submitAll();
                relation_model->select();
                m_selection->setCurrentIndex(relation_model->index(this_row, 0), QItemSelectionModel::ClearAndSelect);
                select_row(this_row);
            }
        }
        record_count->setText(QString("当前表记录条数: %1").arg(relation_model->rowCount()));
    }
}

// 双击表格将弹出对话框，执行更新操作
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(index.isValid()) this->on_action_edit_triggered();
}

// 点击将进行新连接的建立，弹出连接提示框
void MainWindow::on_action_connect_triggered()
{
    first_time_show = true;
    QShowEvent* event = new QShowEvent();
    MainWindow::showEvent(event);
    delete event;
}

// 用户身份核验
bool MainWindow::user_authenticate()
{
    // 打开数据库同时将同时打开身份核验表，以进行登录身份验证
    QSqlQuery query(default_open_database);
    query.prepare("select * from " + default_open_table);
    // query.bindValue(":TABLE", default_open_table);
    // 错误的用法，在query.prepare()语句中尝试绑定表名:TABLE
    // 但是在SQL语句中无法直接绑定表名。绑定参数通常用于绑定值，而不是标识符（如表名、列名等）
    // 应该直接将表名作为字符串拼接到SQL语句中，而不是尝试绑定它。
    if(!query.exec()) {
        QMessageBox::critical(this, "错误", "内部错误, 身份验证表查询失败: " + query.lastError().text());
        return false;
    }
    // 遍历身份表，逐个比对
    while(query.next()) {
        QString query_user = query.value("name").toString();
        QString query_pwd = query.value("password").toString();
//        qDebug() << "输入的user: " << DB.userName();
//        qDebug() << "输入的pwd: " << calculate_sha256(DB.password());
//        qDebug() << "遍历的user: " << query_user;
//        qDebug() << "遍历的pwd: " << query_pwd;
        if(query_user == DB.userName() && query_pwd == calculate_sha256(DB.password())) {
            QMessageBox::information(this, "消息框", "身份认证成功");
            return true;
        }
    }
    QMessageBox::critical(this, "错误", "用户授权验证失败");
    return false;
}

// 登录成功时列出所有数据库
void MainWindow::list_all_databases()
{
    // 展示数据库（展示模型使用的是QStandardItemModel，为了对其中展示的父项和子项进行分类处理）
    database_model = new QStandardItemModel(this);
    QSqlQueryModel* query_model = new QSqlQueryModel();
    QString query = "show databases";
    query_model->setQuery(query);
    // 将借助从queryModel中查询出来的每一个数据库名称作为项插入与treeView关联的standardItemModel中
    for(int row = 0; row < query_model->rowCount(); ++row) {
        QModelIndex index = query_model->index(row, 0);
        QString db_name = query_model->data(index).toString();
        if(is_filtered) {
            // 设置为过滤，则仅显示过滤的数据库名称
            if(db_filter.contains(db_name)) {
                QStandardItem* db = new QStandardItem(db_name);
                db->setIcon(QIcon(":/icons/icons/17.png"));
                database_model->appendRow(db);
            }
            else continue;
        }
        else {
            // 否则全显示
            QStandardItem* db = new QStandardItem(db_name);
            db->setIcon(QIcon(":/icons/icons/17.png"));
            database_model->appendRow(db);
        }
    }
    QStringList labels;
    labels << "";
    database_model->setHorizontalHeaderLabels(labels);
    delete query_model;
    ui->treeView->setModel(database_model);
    QFont font;
    font.setPointSize(12);
    ui->treeView->setFont(font);
}

// 根据所选数据库，列出所有表
void MainWindow::list_tables(const QModelIndex &index)
{
    QSqlQuery query(DB.databaseName());
    if(!query.exec("show tables")) {
        QMessageBox::critical(this, "错误", DB.databaseName() + "数据库中表名生成失败: " + query.lastError().text());
        return;
    }
    QStringList tables;
    while(query.next()) {
        QString table_name = query.value(0).toString();
        if(is_filtered) {
            if(table_filter.contains(table_name)) {
                // 如果启用了表过滤，则仅展示过滤的表名
                tables += table_name;
            }
            else
                continue;
        }
        else
            // 否则全显示
            tables += table_name;
    }
    // 获取到所有表名，将这些表插入对应数据库父项的子目录中（treeView中的项数据库处理需要借助QStandardItem）
    table_list = database_model->itemFromIndex(index);
    // 每次打开时将其已存在的列表清空，否则将堆积起来
    table_list->removeRows(0, table_list->rowCount());
    // 遍历其表，并显示在其下一级目录
    foreach(const QString& table_name, tables) {
        QStandardItem* table = new QStandardItem(table_name);
        table->setIcon(QIcon(":/icons/icons/18.png"));
        // 项插入
        table_list->appendRow(table);
    }
}

// 根据按钮按下与否显示过滤表名
void MainWindow::on_checkBox_specific_clicked(bool checked)
{
    is_filtered = checked;
    // 内存泄漏处理
    memory_leak_check_release();
    // 刷新数据库显示
    list_all_databases();
}

// tableView右键菜单，需要在构造函数初始化右键菜单策略才可使用
void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);
    if(index.isValid()) {
        QMenu* quick_func = new QMenu(this);
        quick_func->addAction(ui->action_append);
        quick_func->addAction(ui->action_delete);
        quick_func->addAction(ui->action_edit);
        quick_func->exec(ui->tableView->mapToGlobal(pos));
    }
}

// 槽函数控制表格selection功能按钮开关
void MainWindow::on_tableItem_move(const QModelIndex &index)
{
    ui->action_next->setEnabled(true);
    ui->action_previous->setEnabled(true);
    ui->action_first->setEnabled(true);
    ui->action_last->setEnabled(true);
    // 点击索引为首行
    if(index.row() == 0) {
        ui->action_previous->setEnabled(false);
        ui->action_first->setEnabled(false);
    }
    // 为末行
    else if(index.row() == index.model()->rowCount() - 1) {
        ui->action_next->setEnabled(false);
        ui->action_last->setEnabled(false);
    }
}

// 表格下一项按钮
void MainWindow::on_action_next_triggered()
{
    // 根据index获取当前选中行行号
    int row = m_selection->currentIndex().row();
    // 自增到下一行
    ++row;
    // 将当前index同步更新到下一行的索引上
    QModelIndex next = m_selection->model()->index(row, 0);
    m_selection->setCurrentIndex(next, QItemSelectionModel::ClearAndSelect);
    // 整行选中
    select_row(row);
    // 按钮功能刷新
    on_tableItem_move(m_selection->currentIndex());
}

// 表格上一项按钮
void MainWindow::on_action_previous_triggered()
{
    // 根据index获取当前选中行行号
    int row = m_selection->currentIndex().row();
    // 自增到上一行
    --row;
    // 将当前index同步更新到上一行的索引上
    QModelIndex prev = m_selection->model()->index(row, 0);
    m_selection->setCurrentIndex(prev, QItemSelectionModel::ClearAndSelect);
    // 整行选中
    select_row(row);
    // 按钮功能刷新
    on_tableItem_move(m_selection->currentIndex());
}

// 表格首项按钮
void MainWindow::on_action_first_triggered()
{
    int row = 0;        // 首行
    // 将当前index同步更新到首行的索引上
    QModelIndex first = m_selection->model()->index(row, 0);
    m_selection->setCurrentIndex(first, QItemSelectionModel::ClearAndSelect);
    // 整行选中
    select_row(row);
    // 按钮功能刷新
    on_tableItem_move(m_selection->currentIndex());
}

// 表格末项按钮
void MainWindow::on_action_last_triggered()
{
    // 根据index获取当前选中行行号
    int row = m_selection->model()->rowCount() - 1;     // 末行
    // 将当前index同步更新到末行的索引上
    QModelIndex last = m_selection->model()->index(row, 0);
    m_selection->setCurrentIndex(last, QItemSelectionModel::ClearAndSelect);
    // 整行选中
    select_row(row);
    // 按钮功能刷新
    on_tableItem_move(m_selection->currentIndex());
}

