#include "filenamefilesystemmodel.h"

FileNameFileSystemModel::FileNameFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{}

QVariant FileNameFileSystemModel::data(const QModelIndex &index, int role) const
{
    // 当展示字段为文件名时，去除其文件后缀(第0列为文件名)
    if(role == Qt::DisplayRole && index.column() == 0) {
        QString file_name = QFileSystemModel::data(index, role).toString();
        int suffix_delim = file_name.lastIndexOf('.');
        // 文件名有效性判断
        if(suffix_delim != -1)
            file_name = file_name.left(suffix_delim);
        return file_name;
    }
    // 自定义表（文件）显示的图标icon
    if(role == Qt::DecorationRole && index.column() == 0) {
        QString file_path = filePath(index);
        QFileInfo file_info(file_path);
        if(file_info.isFile())
            return QIcon(":/icons/icons/4.png");
    }
    // 仅展示该数据库中的表名
    if(role == Qt::DisplayRole && index.column() != 0) {
        return QVariant();
    }
    // 递归处理
    return QFileSystemModel::data(index, role);
}

