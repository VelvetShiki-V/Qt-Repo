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


void Widget::on_pushButton_clean_clicked()
{
    ui->plainTextEdit->clear();
}


void Widget::on_pushButton_front_clicked()
{
    // 获取QString字符串首尾字符测试
    QString title("front & back test\n");
    QString str1 = ui->comboBox_str1->currentText();
    QString ret = QString::asprintf("front: %c\n back: %c",str1.front(), str1.back());

    ui->plainTextEdit->appendPlainText(title);
    ui->plainTextEdit->appendPlainText(ret);
}


void Widget::on_pushButton_left_clicked()
{
    // 获取QString字符串左右字符串测试
    QString title("left & back right test\n");
    QString str1 = ui->comboBox_str1->currentText();
    uint8_t val = ui->spinBox->value();
    QString ret = QString("left-%1: %2\nright-%3: %4").arg(val).arg(str1.left(val)).arg(val).arg(str1.right(val));
    ui->plainTextEdit->appendPlainText(title);
    ui->plainTextEdit->appendPlainText(ret);
}


void Widget::on_pushButton_first_clicked()
{
    // 获取QString字符串前后部分测试
    QString title("first & last right test\n");
    QString str1 = ui->comboBox_str1->currentText();
    QString delim = ui->lineEdit_str2->text();
    uint8_t pos = str1.lastIndexOf(delim);
    QString ret = QString("first file path: %1 \nlast filename: %2") \
   .arg(str1.first(pos)).arg(str1.last(str1.size() - pos - 1));
    ui->plainTextEdit->appendPlainText(title);
    ui->plainTextEdit->appendPlainText(ret);
}


void Widget::on_pushButton_section_clicked()
{
    // 截取QString部分字符串
    QString title("section test\n");
    QString str1 = ui->comboBox_str1->currentText();
    QString delim = ui->lineEdit_str2->text();
    uint8_t pos = ui->spinBox->value();
    QString ret = str1.section(delim[0], pos, pos);
    ui->plainTextEdit->appendPlainText(title);
    ui->plainTextEdit->appendPlainText(ret);

}


void Widget::on_pushButton_null_clicked()
{
    // 字符串无效或空判断
    QString title("null or empty test\n");
    ui->plainTextEdit->appendPlainText(title);

    QString str1 = ui->comboBox_str1->currentText();
    QString str2 = ui->lineEdit_str2->text();
    if(str1.isEmpty()) ui->plainTextEdit->appendPlainText("str1 is empty\n");
    else ui->plainTextEdit->appendPlainText("str1 is not empty\n");
    if(str2.isEmpty()) ui->plainTextEdit->appendPlainText("str2 is empty\n");
    else ui->plainTextEdit->appendPlainText("str2 is not empty\n");
    if(str1.isNull()) ui->plainTextEdit->appendPlainText("str1 is null\n");
    else ui->plainTextEdit->appendPlainText("str1 is not null\n");
    if(str2.isNull()) ui->plainTextEdit->appendPlainText("str2 is null\n");
    else ui->plainTextEdit->appendPlainText("str2 is not null\n");
}


void Widget::on_pushButton_resize_clicked()
{
    // 改大小
    QString title("resize test\n");
    ui->plainTextEdit->appendPlainText(title);

    QString str1 = ui->comboBox_str1->currentText();
    unsigned int size = ui->lineEdit_str2->text().toUInt();
    str1.resize(size, 'X');
    ui->plainTextEdit->appendPlainText("after resize: " + str1);
}


void Widget::on_pushButton_size_clicked()
{
    // 属性展示
    QString title("size length count test\n");
    ui->plainTextEdit->appendPlainText(title);

    QString str1 = ui->comboBox_str1->currentText();
    ui->plainTextEdit->appendPlainText(QString("size: %1").arg(str1.size()));
    ui->plainTextEdit->appendPlainText(QString("length: %1").arg(str1.length()));
    ui->plainTextEdit->appendPlainText(QString("count: %1").arg(str1.count()));
}


void Widget::on_pushButton_trimmed_clicked()
{
    // 字符串空格去除
    QString title("qstring trimmed & simplified test");
    ui->plainTextEdit->appendPlainText(title);

    QString str = ui->lineEdit_str2->text();
    ui->plainTextEdit->appendPlainText(QString("trimmed: %1").arg(str.trimmed()));
    ui->plainTextEdit->appendPlainText(QString("simplified: %1").arg(str.simplified()));
}


void Widget::on_pushButton_insert_clicked()
{
    // 字符串插入
    QString title("qstring insert test");
    ui->plainTextEdit->appendPlainText(title);

    QString str = ui->comboBox_str1->currentText();
    for(int i = 0; i < str.size(); ++i) {
        if(str[i] == '\\')
            str.insert(i++, 'T');       // 注意，insert插入会将字符占据当前下标，插入后需要++，否则死循环
//      qDebug() << str.size();
    }
    ui->plainTextEdit->appendPlainText("after insert: " + str);
}


void Widget::on_pushButton_remove_clicked()
{
    // 字符串擦除
    QString title("qstring remove test");
    ui->plainTextEdit->appendPlainText(title);

    QString str = ui->comboBox_str1->currentText();
    size_t pos = 0;
    while((pos = str.indexOf('\\')) != -1) str.remove(pos, 1);
    ui->plainTextEdit->appendPlainText("after reomve: " + str);
}


void Widget::on_pushButton_replace_clicked()
{
    // 字符串替换
    QString title("qstring remove test");
    ui->plainTextEdit->appendPlainText(title);

    QString str = ui->comboBox_str1->currentText();
    QString rep = "TTT";
    size_t pos = 0;
    while((pos = str.indexOf('\\')) != -1) str.replace(pos, 1, rep);
    ui->plainTextEdit->appendPlainText(str);
}

