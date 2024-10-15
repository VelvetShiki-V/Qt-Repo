#include "tdialogheaderlist.h"
#include "ui_tdialogheaderlist.h"

TDialogHeaderList::TDialogHeaderList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogHeaderList)
{
    ui->setupUi(this);
    m_model = new QStringListModel(this);
    ui->listView->setModel(m_model);
}

TDialogHeaderList::~TDialogHeaderList()
{
    QMessageBox::information(this, "消息框", "headerList对话框已删除");
    delete ui;
}

void TDialogHeaderList::get_current_header_list(const QStringList &header)
{
    // 表头列表以stringlist值初始化
    m_model->setStringList(header);
}

QStringList TDialogHeaderList::set_changed_list()
{
    return m_model->stringList();
}
