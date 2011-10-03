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

#ifndef COLUMNSSUBTITLESVIEW_H
#define COLUMNSSUBTITLESVIEW_H

#include <QTableView>
#include <QMenu>

/**
  * @brief Tableau de sélection de colonnes à afficher pour la liste des sous-titres.
  */
class ColumnsSubtitlesView : public QTableView
{
Q_OBJECT
public:
    explicit ColumnsSubtitlesView(QWidget *parent = 0);
    void setContextMenu(QMenu * menu);

protected:
    void contextMenuEvent ( QContextMenuEvent * e );
    void dropEvent(QDropEvent *event);

private:
    QMenu * m_ctxMenu;
signals:

public slots:

};

#endif // COLUMNSSUBTITLESVIEW_H
