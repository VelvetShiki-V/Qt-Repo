#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置QTreeWidget头部
    ui->treeWidget->clear();
    QTreeWidgetItem* head = new QTreeWidgetItem();
    head->setText(Titem, "目录文件");
    head->setText(Ttype, "类型");
    head->setText(Tdate, "修改日期");
    head->setTextAlignment(Titem, Qt::AlignHCenter | Qt::AlignVCenter);
    ui->treeWidget->setHeaderItem(head);

    // 设置根结点（图片分支）
    QTreeWidgetItem* root_node_pic = new QTreeWidgetItem(Troot);
    root_node_pic->setIcon(Titem, QIcon(":/icons/icons/5.png"));
    root_node_pic->setText(Titem, "图片文件");
    root_node_pic->setText(Ttype, "root TOP");
    root_node_pic->setTextAlignment(Titem, Qt::AlignHCenter | Qt::AlignVCenter);
    root_node_pic->setTextAlignment(Ttype, Qt::AlignHCenter | Qt::AlignVCenter);
    root_node_pic->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);
    root_node_pic->setCheckState(Titem, Qt::Checked);
    ui->treeWidget->addTopLevelItem(root_node_pic);

    // 状态栏
    // 1. 鼠标悬浮信息
    status_info = new QLabel(this);
    status_info->setMinimumWidth(200);
    ui->statusbar->addWidget(status_info);

    // 2. 图片缩放比例（spinbox显示，仅读，无按钮，默认100%）
    ratio_info = new QSpinBox(this);
    ratio_info->setValue(100);
    ratio_info->setSuffix("%");
    ratio_info->setRange(0, 2000);
    ratio_info->setReadOnly(true);
    ratio_info->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->statusbar->addPermanentWidget(ratio_info);

    // 3. 图片路径
    pic_path = new QLabel(this);
    ui->statusbar->addPermanentWidget(pic_path);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 添加目录
void MainWindow::on_action_add_dir_triggered()
{
    // 获取目录完整路径
    QString dir_path = QFileDialog::getExistingDirectory();
    // qDebug() << dir_path;
    if(dir_path.isEmpty()) return;
    QTreeWidgetItem* parent = ui->treeWidget->currentItem();
    if(!parent) parent = ui->treeWidget->topLevelItem(0);       // 默认在最顶层根结点下创建目录
    // 文件夹只能在顶级目录或父目录下递归创建，不能在文件下创建
    if(parent->type() != Tfile) {
        // 获取目录名本身
        QString dir_name = dir_path.right(dir_path.size() - dir_path.lastIndexOf("/") - 1);
        // qDebug() << dir_name;
        QTreeWidgetItem* dir = new QTreeWidgetItem(Tfolder);
        dir->setText(Titem, dir_name);
        dir->setIcon(Titem, QIcon(":/icons/icons/8.png"));
        dir->setText(Ttype, "Folder");
        dir->setTextAlignment(Titem, Qt::AlignHCenter | Qt::AlignVCenter);
        dir->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);
        dir->setCheckState(Titem, Qt::Checked);
        dir->setData(Titem, Qt::UserRole, QVariant(dir_path));      // 初始化？
        parent->addChild(dir);
    }
}

// 创建新文件
void MainWindow::on_action_add_file_triggered()
{
    QStringList file_paths = QFileDialog::getOpenFileNames(this, "选择文件", "", "Images(*.jpg *.png)");
    if(file_paths.isEmpty()) return;
    QTreeWidgetItem* parent, *item;
    // 如果没有选中任何节点，则默认在根结点下创建新的目录或图片文件子节点
    if(!ui->treeWidget->currentItem()) parent = ui->treeWidget->topLevelItem(0);
    else {
        // 若选中节点，则分三种情况（选中为根结点，为目录父节点，为图片叶子结点本身）
        int cur_type = ui->treeWidget->currentItem()->type();
        if(cur_type == Tfile) parent = ui->treeWidget->currentItem()->parent();
        else parent = ui->treeWidget->currentItem();
    }
    if(!parent) return;
    for(int i = 0; i < file_paths.size(); ++i) {
        item = new QTreeWidgetItem(Tfile);
        // 获取图片信息
        QString file = file_paths[i];
        QFileInfo info(file);
        QString filename = info.fileName();
        QDateTime modified = info.lastModified();
        item->setText(Titem, filename);
        item->setText(Ttype, "Image File");
        item->setText(Tdate, modified.toString("yyyy-MM-dd"));
        item->setIcon(Titem, QIcon(":/icons/icons/1.png"));
        item->setTextAlignment(Titem, Qt::AlignHCenter | Qt::AlignVCenter);
        item->setTextAlignment(Ttype, Qt::AlignHCenter | Qt::AlignVCenter);
        item->setTextAlignment(Tdate, Qt::AlignHCenter | Qt::AlignVCenter);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserTristate | Qt::ItemIsUserCheckable);
        item->setCheckState(Titem, Qt::Checked);
        item->setData(Titem, Qt::UserRole, QVariant(file));
        parent->addChild(item);
    }
    parent->setExpanded(true);
}

// 删除节点
void MainWindow::on_action_delete_node_triggered()
{
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(!item) return;
    QTreeWidgetItem* parent = item->parent();
    parent->removeChild(item);
}

// 设置点击item时，按钮的反应
void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(!current || !previous) return;
    // 判断当前结点是否能够（增加目录，增加图片文件，删除节点）
    switch(current->type()) {
    case Troot:
        ui->action_delete_node->setEnabled(false);
        ui->action_add_dir->setEnabled(true);
        ui->action_add_file->setEnabled(true);
        break;
    case Tfolder:
        ui->action_delete_node->setEnabled(true);
        ui->action_add_dir->setEnabled(true);
        ui->action_add_file->setEnabled(true);
        break;
    case Tfile:
        ui->action_delete_node->setEnabled(true);
        ui->action_add_dir->setEnabled(false);
        ui->action_add_file->setEnabled(true);
        ui->action_nice_height->setEnabled(true);
        ui->action_nice_weight->setEnabled(true);
        ui->action_zoom_in->setEnabled(true);
        ui->action_zoom_out->setEnabled(true);
        ui->action_restore->setEnabled(true);
        // 单击图片文件的同时渲染显示该图片
        image_display(ui->treeWidget->currentItem());
        break;
    }
}

// 递归遍历根结点下所有目录或文件
void MainWindow::on_action_node_traversal_triggered()
{
    for(int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        caption_traversal(ui->treeWidget->topLevelItem(i));
}

void MainWindow::caption_traversal(QTreeWidgetItem *item)
{
    if(!item) return;
    item->setText(Titem, QString("*" + item->text(Titem)));
    for(int i = 0; i < item->childCount(); ++i)
        caption_traversal(item->child(i));
}

void MainWindow::image_display(QTreeWidgetItem *item)
{
    // 通过注册好的用户数据获取图片路径属性
    QString file_path = item->data(Titem, Qt::UserRole).toString();
    // qDebug() << file_path;
    // 打印图片名字（状态栏）
    status_info->setText(item->text(Titem));
    // 打印图片路径名（永久状态栏）
    pic_path->setText(file_path);
    // 加载图片到pixmap图片显示流中
    m_pic.load(file_path);
    // 利用label来输出pixmap
    ui->label_picture->setPixmap(m_pic);
}


void MainWindow::on_action_nice_height_triggered()
{
    // 获取需要显示的屏幕尺寸，以及实际尺寸及与其的比例
    int screen_height = ui->scrollArea->height();
    int original_height = m_pic.height();
    int ratio = (float)screen_height / original_height * 100;
    ratio_info->setValue(ratio);
    // 将新尺寸通过pixmap输出到label上
    QPixmap pix = m_pic.scaledToHeight(screen_height - 50);
    ui->label_picture->setPixmap(pix);
}


void MainWindow::on_dockWidget_visibilityChanged(bool visible)
{
    ui->action_window_visible->setChecked(visible);
}


void MainWindow::on_dockWidget_topLevelChanged(bool topLevel)
{
    ui->action_float_window->setChecked(topLevel);
}


void MainWindow::on_action_float_window_triggered(bool checked)
{
    ui->dockWidget->setFloating(checked);
}


void MainWindow::on_action_window_visible_triggered(bool checked)
{
    ui->dockWidget->setVisible(checked);
}


void MainWindow::on_action_zoom_out_triggered()
{
    m_ratio *= 0.8;
    int new_width = ui->label_picture->width() * m_ratio;
    int new_height = ui->label_picture->height() * m_ratio;
    QPixmap pix = m_pic.scaled(new_width, new_height);
    ui->label_picture->setPixmap(pix);
    ratio_info->setValue(m_ratio * 100);
}


void MainWindow::on_action_nice_weight_triggered()
{
    int screen_width = ui->scrollArea->width();
    int original_width = m_pic.width();
    int ratio = (float)screen_width / original_width * 100;
    ratio_info->setValue(ratio);
    QPixmap pix = m_pic.scaledToWidth(screen_width - 50);
    ui->label_picture->setPixmap(pix);
}


void MainWindow::on_action_restore_triggered()
{
    m_ratio = 1;
    ui->label_picture->setPixmap(QPixmap(m_pic.scaledToHeight(m_pic.height())));
    ui->label_picture->setPixmap(QPixmap(m_pic.scaledToWidth(m_pic.width())));
    ratio_info->setValue(m_ratio * 100);
}

void MainWindow::on_action_zoom_in_triggered()
{
    m_ratio *= 1.2;
    int new_width = ui->label_picture->width() * m_ratio;
    int new_height = ui->label_picture->height() * m_ratio;
    QPixmap pix = m_pic.scaled(new_width, new_height);
    ui->label_picture->setPixmap(pix);
    ratio_info->setValue(m_ratio * 100);
}

