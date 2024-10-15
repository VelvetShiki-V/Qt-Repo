#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QActionGroup>
#include <QSpinBox>
#include <QFontComboBox>
#include <QTextCharFormat>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 中英文按钮放入同一动作组——解决互斥性
    QActionGroup *lang = new QActionGroup(this);
    lang->addAction(ui->action_chinese);
    lang->addAction(ui->action_english);

    // 添加其他UI无法插入的组件（需要通过connect联立槽函数和信号）
    // 1. 字体大小调整框
    QSpinBox *font_size_box = new QSpinBox(this);
    font_size_box->setMinimum(5);               // 设置组件属性
    font_size_box->setMaximum(50);
    font_size_box->setValue(ui->textEdit->font().pointSize());
    font_size_box->setMinimumWidth(50);
    ui->toolBar->addWidget(font_size_box);      // 添加组件（自动置入工具栏后方）

    // 2. 字体框
    QFontComboBox *font_style = new QFontComboBox(this);
    font_style->setMinimumWidth(150);
    font_style->setFont(ui->textEdit->font());
    ui->toolBar->addWidget(font_style);
    ui->toolBar->addSeparator();

    // 3.status bar
    status->setMinimumWidth(150);
    status->setText("filename: ");
    ui->statusbar->addWidget(status);

    // connect
    connect(font_size_box, &QSpinBox::valueChanged, this, &MainWindow::on_action_spinbox_fontsizeChanged);
    connect(font_style, &QFontComboBox::currentFontChanged, this, &MainWindow::on_action_combobox_fontstyleChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 4. 文件按钮操作相关（新建，打开，保存）——触发信号：按钮单击
void MainWindow::on_action_new_file_triggered()
{
    ui->textEdit->clear();
    ui->textEdit->document()->setModified(false);       // 创建新文件，不保存
    status->setText("create a new file");
}


void MainWindow::on_action_open_file_triggered()
{
    status->setText("open a file");
}


void MainWindow::on_action_save_file_triggered()
{
    ui->textEdit->document()->setModified(false);
    status->setText("file has been saved");
}

// 5. 文本操作相关（剪切，粘贴，复制）——触发信号——文本可拷贝
void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->action_copy->setEnabled(b);
    ui->action_cut->setEnabled(b);
    ui->action_paste->setEnabled(ui->textEdit->canPaste());     // 剪贴板有就一直保持高亮
}

// 6. 文本样式操作相关（粗体，斜体，下划线）——触发信号：选中文本
void MainWindow::on_textEdit_selectionChanged()
{
   // 获取当前文本框选中文本的字体格式状态
    QTextCharFormat format = ui->textEdit->currentCharFormat();
    // 如果选中的文本已经成为指定样式，则按钮为选中态；否则为抬起态
    ui->action_bold->setChecked(format.font().bold());
    ui->action_italic->setChecked(format.font().italic());
    ui->action_underline->setChecked(format.font().underline());
}


void MainWindow::on_action_bold_triggered(bool checked)
{
    QTextCharFormat format_bold = ui->textEdit->currentCharFormat();
    if(checked) {
        format_bold.setFontWeight(QFont::Bold);
    } else {
        format_bold.setFontWeight(QFont::Normal);
    }
    ui->textEdit->setCurrentCharFormat(format_bold);
}


void MainWindow::on_action_italic_triggered(bool checked)
{
    QTextCharFormat format_italic = ui->textEdit->currentCharFormat();
    format_italic.setFontItalic(checked);
    ui->textEdit->setCurrentCharFormat(format_italic);
}


void MainWindow::on_action_underline_triggered(bool checked)
{
    QTextCharFormat format_underline = ui->textEdit->currentCharFormat();
    format_underline.setFontUnderline(checked);
    ui->textEdit->setCurrentCharFormat(format_underline);
}


// 7. 自定义添加UI组件的槽函数连接（字体框大小调整，样式选择）
void MainWindow::on_action_spinbox_fontsizeChanged(int size)
{
    QTextCharFormat format_size = ui->textEdit->currentCharFormat();
    format_size.setFontPointSize(size);
    ui->textEdit->setCurrentCharFormat(format_size);
}

void MainWindow::on_action_combobox_fontstyleChanged(QFont style)
{
    QTextCharFormat format_style = ui->textEdit->currentCharFormat();
    format_style.setFont(style);
    ui->textEdit->setCurrentCharFormat(format_style);
}

