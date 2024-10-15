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


void Widget::on_pushButton_calculate_clicked()
{
    int amount = ui->lineEdit_amount->text().toInt();
    double price = ui->lineEdit_price->text().toDouble();
    double total = amount * price;
    ui->lineEdit_total->setText(QString::number(total, 10, 2));
}


void Widget::on_pushButton_decimal_clicked()
{
    // 进制转换前先以10进制为基底，再进行转换
    int base_deci = ui->lineEdit_decimal->text().toInt();
    QString bin = QString::number(base_deci, 2);
    QString hex;
    hex.setNum(base_deci, 16);
    ui->lineEdit_binary->setText(bin);
    ui->lineEdit_hex->setText(hex);
}


void Widget::on_pushButton_binary_clicked()
{
    // 进制转换前先以10进制为基底，再进行转换
    bool ok;
    int base_deci = ui->lineEdit_binary->text().toInt(&ok, 2);      // 转换规则为二进制转int
    if(ok) {
        ui->lineEdit_decimal->setText(QString::number(base_deci, 10));
        ui->lineEdit_hex->setText(QString::number(base_deci, 16));
    }
}


void Widget::on_pushButton_hex_clicked()
{
    // 进制转换前先以10进制为基底，再进行转换
    bool ok;
    int base_deci = ui->lineEdit_binary->text().toInt(&ok, 16);
    if(ok) {
        ui->lineEdit_decimal->setText(QString::number(base_deci, 10));
        ui->lineEdit_binary->setText(QString::number(base_deci, 2));
    }
}

