#include "tmainwindowfilesystem.h"
#include "ui_tmainwindowfilesystem.h"

TMainWindowFileSystem::TMainWindowFileSystem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TMainWindowFileSystem)
{
    ui->setupUi(this);
}

TMainWindowFileSystem::~TMainWindowFileSystem()
{
    delete ui;
}

void TMainWindowFileSystem::on_pushButton_clicked()
{
    m_model = new QFileSystemModel(this);
    QString file_path = QCoreApplication::applicationDirPath();
    qDebug() << file_path;
    m_model->setRootPath(file_path);
    ui->treeView->setModel(m_model);
    ui->listView->setModel(m_model);
    ui->tableView->setModel(m_model);
     // **注意，设置rootIndex之前必须先设置将各组件设置好setModel，否则将报错**
    ui->treeView->setRootIndex(m_model->index(file_path));
    ui->listView->setRootIndex(m_model->index(file_path));
    ui->tableView->setRootIndex(m_model->index(file_path));
}


void TMainWindowFileSystem::on_treeView_clicked(const QModelIndex &index)
{
    ui->listView->setRootIndex(index);
    ui->tableView->setRootIndex(index);
}

