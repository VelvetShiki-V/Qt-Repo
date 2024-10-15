#ifndef TMAINWINDOWFILESYSTEM_H
#define TMAINWINDOWFILESYSTEM_H

#include <QMainWindow>
#include <QFileSystemModel>

namespace Ui {
class TMainWindowFileSystem;
}

class TMainWindowFileSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit TMainWindowFileSystem(QWidget *parent = nullptr);
    ~TMainWindowFileSystem();

private slots:
    void on_pushButton_clicked();

    void on_treeView_clicked(const QModelIndex &index);

private:
    QFileSystemModel* m_model;

private:
    Ui::TMainWindowFileSystem *ui;
};

#endif // TMAINWINDOWFILESYSTEM_H
