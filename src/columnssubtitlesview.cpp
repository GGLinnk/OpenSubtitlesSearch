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

#include "columnssubtitlesview.h"
#include "columnssubtitleslistmodel.h"
#include <QContextMenuEvent>
#include <QDebug>
ColumnsSubtitlesView::ColumnsSubtitlesView(QWidget *parent) :
    QTableView(parent),
    m_ctxMenu(NULL)
{
}

/**
  * @brief Définit le menu contextuel à afficher.
  */
void ColumnsSubtitlesView::setContextMenu(QMenu *menu) {
    m_ctxMenu = menu;
}

void ColumnsSubtitlesView::contextMenuEvent(QContextMenuEvent *e) {
    if (!m_ctxMenu) return;
    QModelIndex index = indexAt(e->pos());
    if (index.isValid()) {
        setCurrentIndex(index);
        m_ctxMenu->exec(e->globalPos());
    }
}

void ColumnsSubtitlesView::dropEvent(QDropEvent *event) {
    if (event->proposedAction() == Qt::MoveAction && event->source()) {
        ColumnsSubtitlesListModel * m = (ColumnsSubtitlesListModel *) model();
        QModelIndexList lst = selectionModel()->selectedIndexes();
        if (lst.size() == 1) {
            QModelIndex index = indexAt(event->pos());
            m->moveRow(lst.first().row(), index.row());
        }
    }
}
