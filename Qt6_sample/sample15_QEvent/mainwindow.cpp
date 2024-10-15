#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    grabKeyboard();     // 捕获键盘（方向键也能控制模块移动了）
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 鼠标左键点击事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 基类需要初始化（否则可能产生意想不到的结果）
    QWidget::mousePressEvent(event);
    if(event->button() != Qt::LeftButton) return;
    // 前三者坐标都是相对位置，global是相对于屏幕而言的绝对坐标
    QPoint pos = event->pos();      // 鼠标左键点击确定坐标
    QPointF relative_pos = event->position();
    QPointF scene_pos = event->scenePosition();
    QPointF global_pos = event->globalPosition();

    // 显示需要以坐标的形式输出
    QString display = QString::asprintf("current pos: %d %d\n", pos.rx(), pos.ry());
    display += QString::asprintf("relative pos: %.1f %.1f\n", relative_pos.rx(), relative_pos.ry());
    display += QString::asprintf("scene pos: %.1f %.1f\n", scene_pos.rx(), scene_pos.ry());
    display += QString::asprintf("global pos: %.1f %.1f\n", global_pos.rx(), global_pos.ry());

    ui->label->setText(display);        // 将字符串覆盖显示到label上
    ui->label->move(pos);               // label跟随鼠标点击的实时坐标移动
    ui->label->adjustSize();            // 自动调整大小
}

// 键盘控制模块移动
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    QPoint pos = ui->pushButton->pos();
    if(event->key() == Qt::Key_A || event->key() == Qt::Key_Left)
        ui->pushButton->move(pos.rx() - 20, pos.ry());
    if(event->key() == Qt::Key_S || event->key() == Qt::Key_Down)
        ui->pushButton->move(pos.rx(), pos.ry() + 20);
    if(event->key() == Qt::Key_D || event->key() == Qt::Key_Right)
        ui->pushButton->move(pos.rx() + 20, pos.ry());
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_Up)
        ui->pushButton->move(pos.rx(), pos.ry() - 20);
}

// 该模块主要负责应用的背景图渲染
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap(":/images/images/1.jpg"));
}

// 退出弹出对话框进行二次确认
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ret = QMessageBox::question(this, "消息框", "确定要退出吗",
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if(ret == QMessageBox::Yes)
        event->accept();
    else
        event->ignore();
}

