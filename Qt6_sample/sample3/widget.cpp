#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_unicode_clicked()
{
    QString str = ui->lineEdit_str->text();     // 与font()区别？
    if(str.isEmpty()) return;
    // 将每个字符转换为unicode并输出
    for(const auto& ch: str) {
        char16_t uni = ch.unicode();
        ui->plainTextEdit->appendPlainText(QString::asprintf("%c-->unicode: 0x%X\n", ch, uni));
    }
}


void Widget::on_pushButton_properties_clicked()
{
    QString str = ui->lineEdit_char->text();
    if(str.isEmpty()) return;
    QChar ch = str[0];
//    QString s;
//    str = s.trimmed();
//    int n = s.lastIndexOf("/");
//    s = s.section(str, 1, 1);
//    s.isNull();
//    s.count();
    // 打印unicode
    ui->plainTextEdit->appendPlainText(QString::asprintf("%c-->unicode: 0x%X", ch, ch.unicode()));
    // 设置属性勾选
    ui->chk_digit->setChecked(ch.isDigit());
    ui->chk_letter->setChecked((ch.isLetter()));
    ui->chk_lon->setChecked(ch.isLetterOrNumber());
    ui->chk_lower->setChecked(ch.isLower());
    ui->chk_upper->setChecked(ch.isUpper());
    ui->chk_mark->setChecked(ch.isMark());
    ui->chk_punct->setChecked(ch.isPunct());
    ui->chk_space->setChecked(ch.isSpace());
    ui->chk_symbol->setChecked(ch.isSymbol());
}


void Widget::on_pushButton_cleanup_clicked()
{
    ui->plainTextEdit->clear();
}

