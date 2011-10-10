/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkreplydownloadview.h"
#include "networkreplydownloadtablemodel.h"
#include <QResizeEvent>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

NetworkReplyDownloadView::NetworkReplyDownloadView(QWidget *parent) :
    QTableView(parent)
{
}

void NetworkReplyDownloadView::resizeEvent(QResizeEvent *event) {
    QTableView::resizeEvent(event);
    int row1 = event->size().width() - 30;
    setColumnWidth(0, row1);
    setColumnWidth(1, 30);
}

void NetworkReplyDownloadView::contextMenuEvent(QContextMenuEvent *e) {
    QModelIndex index = indexAt(viewport()->mapFromGlobal(e->globalPos()));
    if (! index.isValid()) return;
    selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
    NetworkReplyDownloadTableModel * m = (NetworkReplyDownloadTableModel *) model();
    const NetworkReplyElem & elem = m->networkReplyElem(index.row());
    if (elem.state != FINISHED) return;
    QMenu * menu = new QMenu(this);
    QAction * action = new QAction(trUtf8("Open subtitle folder"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(openSubtitleFolderUrl()));
    menu->addAction(action);
    action = new QAction(trUtf8("Open subtitle file"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(openSubtitleUrl()));
    menu->addAction(action);
    menu->exec(e->globalPos());
    delete menu;
}

void NetworkReplyDownloadView::openSubtitleUrl() {
    QModelIndexList lst = selectionModel()->selectedIndexes();
    if (lst.isEmpty()) return;
    NetworkReplyDownloadTableModel * m = (NetworkReplyDownloadTableModel *) model();
    const NetworkReplyElem & elem = m->networkReplyElem(lst.first().row());
    Q_ASSERT(elem.state == FINISHED);
    QStringList stlst = elem.text.split("\t\t");
    QDesktopServices::openUrl(QUrl::fromLocalFile(stlst.at(1)));
}

void NetworkReplyDownloadView::openSubtitleFolderUrl() {
    QModelIndexList lst = selectionModel()->selectedIndexes();
    if (lst.isEmpty()) return;
    NetworkReplyDownloadTableModel * m = (NetworkReplyDownloadTableModel *) model();
    const NetworkReplyElem & elem = m->networkReplyElem(lst.first().row());
    Q_ASSERT(elem.state == FINISHED);
    QStringList stlst = elem.text.split("\t\t");
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(stlst.at(1)).dir().path()));
}
