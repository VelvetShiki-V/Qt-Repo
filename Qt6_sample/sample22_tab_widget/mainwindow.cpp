#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tmainwindowfilesystem.h"
#include "tformplaintext.h"
#include "tformwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // mainwindow聚焦中心
    setCentralWidget(ui->tabWidget_2);
    // 嵌入widget状态初始化
    ui->tabWidget_2->setVisible(true);
    ui->tabWidget->clear();
    ui->tabWidget->setVisible(false);
    ui->tabWidget->setTabsClosable(true);       // 需搭配tabWidget的槽函数abCloseRequested一起使用
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_widget_insert_triggered()
{
    // 点击使tabWidget可见
    ui->tabWidget_2->setCurrentIndex(0);
    ui->tabWidget->setVisible(true);
    // 创建窗体，并设置属性
    TFormWidget* text_read = new TFormWidget(this);
    text_read->setAttribute(Qt::WA_DeleteOnClose);
    if(text_read) {
        // 将窗体加入tab，并根据设置索引（根据索引信息来更改tab窗口title）
        int cur_index = ui->tabWidget->addTab(text_read, QString("文本处理框%1").arg(ui->tabWidget->count() + 1));
        ui->tabWidget->setCurrentIndex(cur_index);
        text_read->setVisible(true);
    }
    // 更改窗体名称槽函数
    connect(text_read, &TFormWidget::title_get, this, &MainWindow::change_title);
}


void MainWindow::on_action_widget_indep_triggered()
{
    TFormWidget* text_read = new TFormWidget(this);
    text_read->setAttribute(Qt::WA_DeleteOnClose);
    if(text_read) {
        text_read->setWindowTitle("文本读取框");
        text_read->show();
    }
}

void MainWindow::change_title(const QString &title)
{
    int index = ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(index, title);
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->widget(index)->close();
}

// 为mainWindow设置背景图
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, ui->toolBar->height() + ui->menuBar->height(), this->width(), this->height(), QPixmap("D:\\File\\图集\\涩涩\\美图6\\9.jpg"));
}

void MainWindow::on_action_mainwindow_insert_triggered()
{
    ui->tabWidget_2->setCurrentIndex(0);
    ui->tabWidget->setVisible(true);
    TMainWindowFileSystem* fs = new TMainWindowFileSystem(this);
    fs->setAttribute(Qt::WA_DeleteOnClose);
    if(fs) {
        ui->tabWidget->addTab(fs, QString("文件系统浏览%1").arg(ui->tabWidget->count() + 1));
        fs->setVisible(true);
    }
}


void MainWindow::on_action_mainwindow_indep_triggered()
{
    TMainWindowFileSystem* fs = new TMainWindowFileSystem(this);
    fs->setAttribute(Qt::WA_DeleteOnClose);
    if(fs) {
        fs->setWindowTitle("文件系统浏览");
        fs->show();
    }
}


void MainWindow::on_action_new_widget_triggered()
{
    ui->tabWidget_2->setCurrentIndex(1);
    ui->action_close_all->setEnabled(true);
    TFormPlainText* window = new TFormPlainText(this);
    ui->mdiArea->addSubWindow(window);
    window->show();
    ui->action_font->setEnabled(true);
    ui->action_cascade->setEnabled(true);
    ui->action_tile->setEnabled(true);
    ui->actionMDI->setEnabled(true);
    connect(window, &TFormPlainText::text_selected, this, &MainWindow::content_selected);
    connect(window, &TFormPlainText::text_changed, this, &MainWindow::content_changed);
}


void MainWindow::on_action_open_widget_triggered()
{
    ui->tabWidget_2->setCurrentIndex(1);
    TFormPlainText* cur_window;
    if(ui->mdiArea->subWindowList().size() > 0) {
        QWidget* window = ui->mdiArea->currentSubWindow()->widget();
        cur_window = dynamic_cast<TFormPlainText*>(window);
    }
    else {
        cur_window = new TFormPlainText(this);
        ui->mdiArea->addSubWindow(cur_window);
    }
    cur_window->load_file();
    cur_window->show();
    // 当当前窗体存在已有文件时，保存当前文件并打开新文件
//    connect(cur_window, )
    ui->action_font->setEnabled(true);
    ui->action_cascade->setEnabled(true);
    ui->action_tile->setEnabled(true);
    ui->actionMDI->setEnabled(true);
    connect(cur_window, &TFormPlainText::text_selected, this, &MainWindow::content_selected);
    connect(cur_window, &TFormPlainText::text_changed, this, &MainWindow::content_changed);
}


void MainWindow::on_action_font_triggered()
{
    TFormPlainText* cur_window = dynamic_cast<TFormPlainText*>(ui->mdiArea->currentSubWindow()->widget());
    cur_window->font();
}


void MainWindow::on_action_cut_triggered()
{
    TFormPlainText* cur_window = dynamic_cast<TFormPlainText*>(ui->mdiArea->currentSubWindow()->widget());
    cur_window->cut();
}


void MainWindow::on_action_copy_triggered()
{
    TFormPlainText* cur_window = dynamic_cast<TFormPlainText*>(ui->mdiArea->currentSubWindow()->widget());
    cur_window->copy();
}


void MainWindow::on_action_paste_triggered()
{
    TFormPlainText* cur_window = dynamic_cast<TFormPlainText*>(ui->mdiArea->currentSubWindow()->widget());
    cur_window->paste();
}


void MainWindow::on_action_cascade_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}


void MainWindow::on_action_tile_triggered()
{
    ui->mdiArea->tileSubWindows();
}


void MainWindow::on_action_close_all_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::content_selected()
{
    ui->action_copy->setEnabled(true);
    ui->action_cut->setEnabled(true);
    ui->action_paste->setEnabled(true);
}

void MainWindow::content_changed()
{
    ui->action_save->setEnabled(true);
}


void MainWindow::on_action_save_triggered()
{
    TFormPlainText* cur_window = dynamic_cast<TFormPlainText*>(ui->mdiArea->currentSubWindow()->widget());
    cur_window->save();
}


void MainWindow::on_actionMDI_triggered(bool checked)
{
    if(checked)
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    else
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
}

