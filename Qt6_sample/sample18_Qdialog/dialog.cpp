#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_open_file_clicked()
{
    // 先获取当前可执行程序路径，再选择获取需要打开的文件路径
    QString app_path = QCoreApplication::applicationFilePath();
    QString file_path = QFileDialog::getOpenFileName(this, "选择一个文件", app_path,
                                                    "文本文件(*.txt);;图片文件(*.png *.jpg *.gif);;所有文件(*.*)");
    QFileInfo info(file_path);      // 通过该类获取文件名
    if(file_path.isEmpty()) return;
    ui->plainTextEdit->appendPlainText("获取到打开的文件路径: " + file_path);
    ui->plainTextEdit->appendPlainText("获取到打开的文件名: " + info.fileName());
}


void Dialog::on_pushButton_open_files_clicked()
{
    QString app_path = QCoreApplication::applicationFilePath();
    QStringList files_path = QFileDialog::getOpenFileNames(this, "选择多个文件", app_path,
                                                    "文本文件(*.txt);;图片文件(*.png *.jpg *.gif);;所有文件(*.*)");
    for(const auto& e: files_path) {
        ui->plainTextEdit->appendPlainText("获取到打开的文件路径: " + e);
    }
}


void Dialog::on_pushButton_select_dir_clicked()
{
    QString app_path = QCoreApplication::applicationFilePath();
    QString dir_path = QFileDialog::getExistingDirectory(this, "选择目录", app_path);
    if(dir_path.isEmpty()) return;
    ui->plainTextEdit->appendPlainText("获取到目录选择的路径: " + dir_path);
}


void Dialog::on_pushButton_save_file_clicked()
{
    QString app_path = QCoreApplication::applicationFilePath();
    QString save_path = QFileDialog::getSaveFileName(this, "选择需要另存为的目录或文件", app_path,
    "文本文件(*.txt);;图片文件(*.png *.jpg *.gif);;所有文件(*.*)");
    if(save_path.isEmpty()) return;
    ui->plainTextEdit->appendPlainText("获取到保存的文件路径: " + save_path);
}


void Dialog::on_pushButton_select_color_clicked()
{
    // 参数先获取QColor类型的初始化值，dialog调出颜色对话框，再通过palette设置颜色
    QPalette pal = ui->plainTextEdit->palette();
    QColor init_color = pal.color(QPalette::Text);
    QColor font_color = QColorDialog::getColor(init_color, this, "选择颜色");
    if(font_color.isValid()) {
        pal.setColor(QPalette::Text, font_color);
        ui->plainTextEdit->setPalette(pal);
    }
}


void Dialog::on_pushButton_select_font_clicked()
{
    bool flag;
    QFont font = QFontDialog::getFont(&flag, this);
    if(flag) {
        ui->plainTextEdit->setFont(font);
    }
}


void Dialog::on_pushButton_progress_clicked()
{
    // 进度条对话框初始化
    QProgressDialog progress("复制中", "取消", 0, 100, this);
    progress.setWindowTitle("文本复制");
    progress.setWindowModality(Qt::WindowModal);
    // 槽函数当进度条按钮取消被触发时，将打印到文本框上
    connect(&progress, &QProgressDialog::canceled, [&](){ui->plainTextEdit->appendPlainText("进度已取消");});
    QElapsedTimer counter;
    for(int i = 0; i < 100; ++i) {
        counter.start();
        progress.setValue(i);       // 自带显示条目进度栏
        progress.setLabelText(QString("text copying percent: %1 %").arg(i));    // 辅助显示（可不用）
        Sleep(100);     // 如果不加入该语句，则进度栏会一瞬间结束，观察不到效果
//        while(1) {
//            if(counter.elapsed() > 80) break;     // 等效于Sleep
//        }
        if(progress.wasCanceled()) break;
    }
}


void Dialog::on_pushButton_input_str_clicked()
{
    // lineedit输入模式，normal为普通输入显示，password为密码输入模式
    QLineEdit::EchoMode mode = QLineEdit::Normal;
    bool flag;
    QString str = QInputDialog::getText(this, "字符串输入框", "输入字符串", mode, "please input here", &flag);
    if(flag && !str.isEmpty())
        ui->plainTextEdit->appendPlainText(str);
}


void Dialog::on_pushButton_input_int_clicked()
{
    bool flag;
    int val = QInputDialog::getInt(this, "数值输入框", "输入数值", 0, 0, 20000, 1, &flag);
    if(flag)
        ui->plainTextEdit->appendPlainText(QString::number(val));
}


void Dialog::on_pushButton_input_float_clicked()
{
    bool flag;
    double val = QInputDialog::getDouble(this, "数值输入框", "输入数值", 0.00, 0.00, 20000.00, 2, &flag);
    if(flag)
        ui->plainTextEdit->appendPlainText(QString::number(val));
}


void Dialog::on_pushButton_combo_clicked()
{
    QStringList items;
    items << "优" << "良" << "中" << "差";
    bool flag;
    QString content = QInputDialog::getItem(this, "选择框", "项目文本选择", items, 0, false, &flag);
    if(flag) {
        ui->plainTextEdit->appendPlainText(content);
    }
}


void Dialog::on_pushButton_question_clicked()
{
    int ret = QMessageBox::question(this, "询问消息框", "是否打开文件", QMessageBox::Yes | QMessageBox::No |
                                                QMessageBox::Cancel | QMessageBox::NoButton);
    if(ret == QMessageBox::Yes)
        ui->plainTextEdit->appendPlainText("yes was clicked");
    else if(ret == QMessageBox::No)
        ui->plainTextEdit->appendPlainText("No was clicked");
    else if(ret == QMessageBox::Cancel)
        ui->plainTextEdit->appendPlainText("Cancel was clicked");
    else
        ui->plainTextEdit->appendPlainText("NoButton was clicked");
}


void Dialog::on_pushButton_information_clicked()
{
    QMessageBox::information(this, "消息框", "这是一个提示消息" );
}


void Dialog::on_pushButton_warning_clicked()
{
    QMessageBox::warning(this, "警告框", "这是一个警告");
}


void Dialog::on_pushButton_critical_clicked()
{
    QMessageBox::critical(this, "404", "stack overflow!");
}


void Dialog::on_pushButton_about_clicked()
{
    QMessageBox::about(this, "关于版本", "ver.1.1.2");
}


void Dialog::on_pushButton_aboutQT_clicked()
{
    QMessageBox::aboutQt(this, "关于版本");
}

