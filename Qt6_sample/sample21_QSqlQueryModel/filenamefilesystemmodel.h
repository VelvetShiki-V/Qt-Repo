#ifndef FILENAMEFILESYSTEMMODEL_H
#define FILENAMEFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QObject>

class FileNameFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileNameFileSystemModel(QObject *parent = nullptr);
    // 文件系统在treeView中展示文件，重写的展示数据格式
    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // FILENAMEFILESYSTEMMODEL_H
