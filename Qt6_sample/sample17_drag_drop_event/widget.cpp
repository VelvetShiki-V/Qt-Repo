#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 接收文件拖拽，但禁止每个组件都单独接收拖拽文件，而是进行统一处理
    setAcceptDrops(true);
    ui->plainTextEdit->setAcceptDrops(false);
    ui->label->setAcceptDrops(false);
    ui->label->setScaledContents(true);
}

Widget::~Widget()
{
    delete ui;
}

// 管理拖拽事件（只接收指定后缀的文件）
void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        // mimeData获取可拖拽式多媒体文件信息，urls列表获取属性，第0行为文件绝对路径（最面前带斜杠\）
        QString filename = event->mimeData()->urls()[0].fileName();
        qDebug() << "file url: " << event->mimeData()->urls();
        qDebug() << "filename: " << filename;
        QFileInfo fileInfo(filename);
        QString suffix = fileInfo.suffix().toLower();
        if(suffix == "jpg") event->acceptProposedAction();
        else event->ignore();
    }
    else event->ignore();
}

// 管理拖拽后文件的信息存入事件
void Widget::dropEvent(QDropEvent *event)
{
    QString full_path = event->mimeData()->urls()[0].path();
    QString file_path = full_path.right(full_path.length() - url_prefix_count);
    qDebug() << "pic full path:" << full_path;
    qDebug() << "file path:" << file_path;
    // 将url结构体信息打印到plaintexteditor
    ui->plainTextEdit->clear();
    for(const auto& e: event->mimeData()->urls()) {
        ui->plainTextEdit->appendPlainText(e.path());
    }
    ui->plainTextEdit->appendPlainText("file path: " + file_path);
    // 将拖入的多媒体图片文件通过pixmap渲染输出到label
    QPixmap pix(file_path);
    ui->label->setPixmap(pix);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QSize plainText_size = ui->plainTextEdit->size();
    // move控制widget组件与窗口的外间距
    ui->plainTextEdit->move(5, 5);
    ui->label->move(5, plainText_size.height() + 10);
    // resize控制当窗口大小有所调整时，组件大小将重新调整
    ui->plainTextEdit->resize(this->width() - 10, plainText_size.height());     // 宽度随着窗口变，高度不变
    // 宽高均随着窗口改变
    ui->label->resize(this->width() - 10, this->height() - plainText_size.height() - 20);
    // 将窗口大小改变的事件应用
    event->accept();
}
