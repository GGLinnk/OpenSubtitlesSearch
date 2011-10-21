#include "filesystemview.h"

FileSystemView::FileSystemView(QWidget *parent) :
    QTreeView(parent)
{
    setModel(new QFileSystemModel(this));
}
