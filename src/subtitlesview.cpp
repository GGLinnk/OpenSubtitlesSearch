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

#include "subtitlesview.h"
#include <QContextMenuEvent>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <QDesktopServices>
#include <QMenu>
#include "subtitleslistmodel.h"

SubtitlesView::SubtitlesView(QWidget *parent) :
    QTableView(parent),
    m_hasSelection(false),
    m_hasSingleSelection(false),
    m_contextMenu(NULL)
{
    QSortFilterProxyModel * pm = new QSortFilterProxyModel(this);
    pm->setSourceModel(new SubtitlesListModel(this));
    QTableView::setModel(pm);
}

QSortFilterProxyModel * SubtitlesView::proxyModel() {
    return (QSortFilterProxyModel *) model();
}

SubtitlesListModel * SubtitlesView::sourceModel() {
    return (SubtitlesListModel *) ((QSortFilterProxyModel *) model())->sourceModel();
}

const QList<QVariant> & SubtitlesView::subtitlesData() {
    return sourceModel()->subtitlesData();
}

QMap<QString, QVariant> SubtitlesView::subtitlesData(const QModelIndex & index) {
    return sourceModel()->subtitlesData(proxyModel()->mapToSource(index).row());
}

QStringList SubtitlesView::selectedSubtitlesValue(const QString & key) {
    QStringList result;
    QModelIndexList irows = selectionModel()->selectedRows(0);
    foreach (QModelIndex i, irows) {
        result << subtitlesData(i).value(key).toString();
    }
    return result;
}

void SubtitlesView::addSubtitlesData(const QList<QVariant> & data) {
    sourceModel()->addSubtitlesData(data);
}

void SubtitlesView::clearSubtitlesData() {
    sourceModel()->clearSubtitlesData();
}

QMap<QString, QVariant> SubtitlesView::subtitlesData(const QString & key, const QString & value) {
    return sourceModel()->subtitleData(key, value);
}

void SubtitlesView::setColumns(const QStringList & columns) {
    sourceModel()->setColumns(columns);
}

void SubtitlesView::openCurentItemUrl(const QString &dataKey) {
    if (! currentIndex().isValid()) return;
    QMap<QString, QVariant> data = subtitlesData(currentIndex());
    QUrl url(data.value(dataKey).toString());
    QDesktopServices::openUrl(url);
}

void SubtitlesView::openDownloadGzLink() {
    openCurentItemUrl("SubDownloadLink");
}

void SubtitlesView::openDownloadZipLink() {
    openCurentItemUrl("ZipDownloadLink");
}

void SubtitlesView::openShowLink() {
    openCurentItemUrl("SubtitlesLink");
}

void SubtitlesView::contextMenuEvent ( QContextMenuEvent * e ) {
    if (m_contextMenu) m_contextMenu->exec(e->globalPos());
}

void SubtitlesView::selectionChanged(const QItemSelection &selected, const QItemSelection & deselected) {
    QTableView::selectionChanged(selected, deselected);
    bool bSelection = selectionModel()->hasSelection();
    if (bSelection != m_hasSelection) {
        m_hasSelection = bSelection;
        emit hasSelection(bSelection);
    }

    if (bSelection) {
        bool singleSelection = selectionModel()->selectedRows().size() == 1;
        if (singleSelection != m_hasSingleSelection) {
            m_hasSingleSelection = singleSelection;
            emit hasSingleSelection(singleSelection);
        }
    }
}


