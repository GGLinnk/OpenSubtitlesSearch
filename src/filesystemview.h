#ifndef FILESYSTEMVIEW_H
#define FILESYSTEMVIEW_H

#include <QTreeView>
#include <QFileSystemModel>

class FileSystemView : public QTreeView
{
    Q_OBJECT
public:
    explicit FileSystemView(QWidget *parent = 0);

    inline QFileSystemModel * fsmodel() { return (QFileSystemModel *) model(); }

signals:

public slots:

};

#endif // FILESYSTEMVIEW_H
