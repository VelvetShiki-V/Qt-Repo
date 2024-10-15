#include "tformwidget.h"
#include "ui_tformwidget.h"

TFormWidget::TFormWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormWidget)
{
    ui->setupUi(this);
    // 设置窗口标志（标题栏，如果没有则可能无边框）
    setWindowFlags(Qt::Window | Qt::WindowTitleHint);
}

TFormWidget::~TFormWidget()
{
    delete ui;
}

void TFormWidget::on_pushButton_clicked()
{
    QString app_path = QCoreApplication::applicationDirPath();
    QString file_path = QFileDialog::getOpenFileName(this, "选择文件", app_path, "全部文件(*.*)");
    QFile text_file(file_path);
    if(app_path.isEmpty() || file_path.isEmpty() ||
        !text_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "文件读取失败");
        return;
    }
    // 文件打开成功，开始读取数据
    QTextStream stream(&text_file);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(stream.readAll());
    // 将打开的文件名获取并发送给tab
    QFileInfo file_info(file_path);
    QString title = file_info.fileName();
    emit title_get(title);
    // 关闭文件
    text_file.close();
}

