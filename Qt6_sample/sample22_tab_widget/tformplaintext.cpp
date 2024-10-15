#include "tformplaintext.h"
#include "ui_tformplaintext.h"

TFormPlainText::TFormPlainText(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TFormPlainText)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint);
    setWindowTitle("空白文本框");
}

TFormPlainText::~TFormPlainText()
{
    delete ui;
}

void TFormPlainText::load_file()
{
    QString app_path = QCoreApplication::applicationDirPath();
    file_path = QFileDialog::getOpenFileName(this, "选择文件", app_path, "全部文件(*.*)");
    QFile get_file(file_path);
    if(!get_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream stream(&get_file);
    ui->plainTextEdit->clear();
    ui->plainTextEdit->setPlainText(stream.readAll());
    get_file.close();
    // 根据打开文件设置标题
    QFileInfo info(file_path);
    setWindowTitle(info.fileName());
}

void TFormPlainText::save()
{
    QFile opened_file(file_path);
    if(!opened_file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
     // 流插入方法一
//    QTextStream stream(&opened_file);
//    stream << ui->plainTextEdit->toPlainText();
    // 文件写入方法二
    QByteArray stream = ui->plainTextEdit->toPlainText().toUtf8();
    opened_file.write(stream);
    opened_file.close();
    QMessageBox::information(this, "信息", "保存成功");
    QFileInfo info(file_path);
    QString file_name = info.fileName();
    setWindowTitle(file_name);
}

void TFormPlainText::cut()
{
    ui->plainTextEdit->cut();
}

void TFormPlainText::copy()
{
    ui->plainTextEdit->copy();
}

void TFormPlainText::paste()
{
    ui->plainTextEdit->paste();
}

void TFormPlainText::font()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->plainTextEdit);
    if(ok)
        ui->plainTextEdit->setFont(font);
}

void TFormPlainText::on_plainTextEdit_selectionChanged()
{
    emit text_selected();
}


void TFormPlainText::on_plainTextEdit_modificationChanged(bool arg1)
{
    Q_UNUSED(arg1);
    QFileInfo info(file_path);
    QString file_name = info.fileName();
    setWindowTitle(file_name + "*");
    emit text_changed();
}

