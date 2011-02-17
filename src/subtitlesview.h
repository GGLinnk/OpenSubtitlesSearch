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

#ifndef SUBTITLESVIEW_H
#define SUBTITLESVIEW_H

#include <QTableView>

class QSortFilterProxyModel;
class SubtitlesListModel;
class QMenu;

class SubtitlesView : public QTableView
{
Q_OBJECT
public:
    explicit SubtitlesView(QWidget *parent = 0);
    const QList<QVariant> & subtitlesData();
    QMap<QString, QVariant> subtitlesData(const QModelIndex & index);

    QStringList selectedSubtitlesValue(const QString & key);

    void addSubtitlesData(const QList<QVariant> & data);
    void clearSubtitlesData();

    QMap<QString, QVariant> subtitlesData(const QString & key, const QString & value);
    void setColumns(const QStringList & columns);

    inline void setContextMenu(QMenu * menu) { m_contextMenu = menu; }

protected:
    void contextMenuEvent ( QContextMenuEvent * e );
    QSortFilterProxyModel * proxyModel();
    SubtitlesListModel * sourceModel();
    void openCurentItemUrl(const QString & dataKey);

signals:
    void hasSelection(bool selection);
    void hasSingleSelection(bool selection);

protected slots:
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void openDownloadGzLink();
    void openDownloadZipLink();
    void openShowLink();

private:
    bool m_hasSelection, m_hasSingleSelection;
    QMenu * m_contextMenu;
};

#endif // SUBTITLESVIEW_H
